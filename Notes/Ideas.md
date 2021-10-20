- Stenen
	- Kiezels / Baksteenafval / ...
	- vul $I_T$ met overschot 
	- grid is afhankelijk van de vorm van baksteen
	- kleur van baksteen bepaalt de plaats 
	- cutting niet mogelijk, bakstenen zijn bros

- 3D
	- Using textured strips to represent textured 3D model $\approx$ 3D Woodpixel, perhaps cut ribbons out of textured plane -> combine 3D weaving, foldable patches, texture mapping, ...
		- [Zippable ribbons](https://arxiv.org/abs/1711.02450)
		- [[Curved ribbons.pdf|3D Weaving with Curved Ribbons]]
		- [The Design Space of Plane Elastic Curves](http://visualcomputing.ist.ac.at/publications/2021/TDSOPEC/)
	- [Peeling](http://staff.ustc.edu.cn/~fuxm/projects/Peeling/index.html)
		- Start with a textured 3D mesh and create cutlines
		- After exploding / peeling the mesh, we recreated the target image
	- [[Surface to volume.pdf|Surface to volume]]
	- [[Fabric Form Work.pdf|Creating patches that cover a 3D model]]
	- [[String Actuated Curved Folded Surfaces.pdf|Folding and cutting textured paper to recreate a 3D shape]]
	- [[Geodesic grid structures.pdf]]

- Input materials
	- Can be existing materials
		- Wood
			- Can be cut
		- Stones
			- No cutting possible
		- Bricks
			- Almost no cutting possible
		- Fruit / vegetables [[Vegetables.png]]
			- Limited cutting possible
	- Can be artistic 
		- Ink
		- Paint splash [[Paint splash.png]]
		- Watercolor splash [[Watercolor splash.png]]

- Grid constraints
	- Periodic
		- Regular rectangular / triangular
		- Honeycomb
		- Quadtree [[Quadtree.png]]
	- Not periodic
		- Voronoi [[Voronoi.png]]
		- Mondriaan -> combine with [stylization](https://ondrejtexler.github.io/patch-based_training/)?
	- Escherization

- Art styles
	- Mondriaan [[Mondriaan.png]]
	- Cubism [[Cubism.png]]
	- [Gilmore leaves](https://www.shadertoy.com/view/MsdSWl)

- Multiple targets
	- Use a single source to create tiles for multiple target images

- Interests
	- Shadow art design
	- Optical illusions
	- Stylization