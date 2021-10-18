 # WoodPixel
### How?
Data acquisition --> Data analysis --> Cut pattern generation

## Data acquisition
#### Overview
1. Thin veneers glued to substrate -> burl veneers tend to be brittle
2. Sanding and coating / oiling to enhance contrast of veneers
3. Scanned at 300 dpi -> aligned for a common coordinate frame
4. Mask created to separate veneer from background and screw holes

#### Input: 
- Veneers
#### Output:
- Set of source textures $I_S=\{I_{S,i}: i \in \{1,\cdots,n_{samples}\}\}$, one for each physical wood sample

## Feature extraction
- Find patch correspondences between target and [[#Output|source]] images -> define representation for textural structures within the patches
- Densely evaluate using a filter bank consisting of 2 image filters: an [[Intensity filter|intensity]] and a [[Sobel filter|Sobel]] edge filter. $$\textbf{F}(\textbf{I})=\left(w_{intensity} \cdot \textbf{F}_{intensity}(\textbf{I}), \; w_{edge} \cdot \textbf{F}_{edge}(\textbf{I})\right)^\top$$ $$w_x \in [0, 1] \quad and \quad x \in \{intensity, edge\}$$
- The weights allow artistic control over the emphasis on overall intensity matching and find scale gradient features.
- We take $n_{rot}$ rotated versions of the wood source textures to increase the probability of finding good matches
- Source and target images may have different gamuts and response distributions -> apply CDF-based histogram equalization to transform the intensity distribution of the target image to that of the available source textures.
- ... [[Todo]]

####  Output
A set of $n_{samples} \cdot n_{rot}$ filter responses for the source textures $$\textbf{F}(\textbf{I}_S)=\{\textbf{F}(\textbf{I}_{S,i,\phi_j}):i \in \{1,\cdots,n_{samples}, j \in \{1,\cdots,n_{rot}\}\}\}$$ and one filter response $\textbf{F}(\textbf{I}_T)$.

## Cut pattern optimization