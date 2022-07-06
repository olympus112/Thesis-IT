#pragma once

#include <unordered_map>
#include <optional>
#include <vector>
#include <cstdint>
#include <cassert>
#include <execution>


template <typename>
struct btree_traits {
	using id_type = std::uint32_t;
	static constexpr id_type null_node = 0xffffffff;
};

template <typename Node>
struct BNode {
	using traits = btree_traits<Node>;
	using id_type = typename traits::id_type;

public:
	id_type count;

	id_type index;
	id_type parent;

	id_type left;
	id_type right;

	id_type next;

	BNode() {
		this->parent = traits::null_node;
		this->left = traits::null_node;
		this->right = traits::null_node;
		this->next = traits::null_node;
		this->count = 0;
	}

	BNode(id_type index)
		: BNode() {
		this->index = index;
	}

	bool leaf() const {
		return left == traits::null_node;
	}


	bool unary() const {
		return left != traits::null_node && right == traits::null_node;
	}

	bool binary() const {
		return left != traits::null_node && right != traits::null_node;
	}
};

template <typename Node, typename Context = void>
struct BTree {
private:
	Context* context;

	using traits = btree_traits<Node>;
	using id_type = typename traits::id_type;

public:
	id_type root;
	std::vector<BNode<Node>> nodes;

public:
	BTree(Context* context = nullptr, std::size_t nodes = 0) {
		this->nodes.reserve(nodes * 2);

		this->context = context;
		this->root = createNode(std::nullopt).index;
	}

	BNode<Node>& createNode(const std::optional<Node>& value) {
		// Create node
		id_type nodeIndex = nodes.size();
		BNode<Node>& node = nodes.template emplace_back(nodeIndex);

		// Early return if node is empty
		if (!value.has_value())
			return node;

		// Set node bounds
		if (value.has_value()) {
			// Set bounds
			node.bounds = getBounds(this->context, *value);

			// Set node value
			node.value = value;

			// Set node count
			node.count = 1;
		}

		return node;
	}

	std::pair<BNode<Node>&, BNode<Node>&> branch(BNode<Node>& parentNode,
	                                             const std::optional<Node>& leftValue,
	                                             const std::optional<Node>& rightValue) {
		// Create new nodes
		BNode<Node>& leftNode = createNode(leftValue);
		BNode<Node>& rightNode = createNode(rightValue);

		// Set children
		parentNode.right = rightNode.index;
		parentNode.left = leftNode.index;

		// Set parent
		rightNode.parent = parentNode.index;
		leftNode.parent = parentNode.index;

		// Set next
		parentNode.next = leftNode.index;
		leftNode.next = rightNode.index;

		return std::pair<BNode<Node>&, BNode<Node>&>(leftNode, rightNode);
	}

	void update(BNode<Node>& node) {
		if (node.parent == traits::null_node)
			return;

		BNode<Node>& parent = nodes[node.parent];

		if (parent.unary()) {
			BNode<Node>& leftNode = nodes[parent.left];

			parent.bounds = leftNode.bounds;
			parent.count = leftNode.count;

			update(parent);
		} else if (parent.binary()) {
			BNode<Node>& leftNode = nodes[parent.left];
			BNode<Node>& rightNode = nodes[parent.right];

			parent.bounds = leftNode.bounds.expanded(rightNode.bounds);
			parent.count = leftNode.count + rightNode.count;

			update(parent);
		}
	}

	Node& insert(id_type nodeIndex, const Node& value) {
		BNode<Node>& parentNode = nodes[nodeIndex];

		// Move new value to empty leaf node
		if (parentNode.leaf() && parentNode.empty()) {
			// Set node value
			parentNode.value = value;

			// Set node count
			parentNode.count = 1;

			// Set node bounds
			parentNode.bounds = getBounds(this->context, value);

			// Recursively update bounds and count
			update(parentNode);

			return *parentNode.value;
		}

		assert(parentNode.empty() || parentNode.leaf());

		if (!parentNode.empty()) {
			// Move parent value to left node and new value to right node
			id_type originalNext = parentNode.next;

			// Create left and right branch
			auto [leftNode, rightNode] = branch(parentNode, parentNode.value, value);

			// Reset parent value
			parentNode.value = std::nullopt;

			// Set right node next
			rightNode.next = originalNext;

			// Update count and bounds recursively
			update(rightNode);

			return *rightNode.value;
		} else {
			// Pass new value to left or right node
			BNode<Node>& leftNode = nodes[parentNode.left];
			BNode<Node>& rightNode = nodes[parentNode.right];

			Bounds nodeBounds = getBounds(this->context, value);
			Bounds newLeftBounds = leftNode.bounds.expanded(nodeBounds);
			Bounds newRightBounds = rightNode.bounds.expanded(nodeBounds);

			if constexpr (Split == BTreeSplitMethod_SurfaceArea) {
				double newLeftSurface = newLeftBounds.surface();
				double newRightSurface = newRightBounds.surface();

				double newLeftCost = newLeftSurface * leftNode.count;
				double newRightCost = newRightSurface * rightNode.count;

				if (newLeftCost <= newRightCost)
					return insert(parentNode.left, value);
				else
					return insert(parentNode.right, value);
			}
		}
	}

	Node& insert(const Node& value) {
		id_type size = nodes.size() + 2;
		nodes.reserve(size);

		return insert(root, value);
	}
};
