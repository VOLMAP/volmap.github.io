# VOLMAP: a Large Scale Benchmark for Volume Mappings to Simple Base Domains

<p align="center"><img src="teaser.png"></p>

VOLMAP is a large scale benchmark aimed to support ongoing research in volume mapping algorithms. The dataset contains **4.7K tetrahedral meshes**, whose boundary vertices are mapped to a variety of simple domains, either convex or star-shaped. This data constitutes the input for candidate algorithms, which are then required to position interior vertices in the domain to obtain a volume map. Overall, this yields **more than 22K alternative test cases**. VOLMAP also comprises tools to process this data, analyze the resulting maps, and extend the dataset with new meshes, boundary maps and base domains.

Dataset and tools are described in details in the article:<br>
[***VOLMAP: a Large Scale Benchmark for Volume Mappings to Simple Base Domains***]()<br>
Computer Graphics Forum (SGP 2023)<br>
G. Cherchi and M. Livesu<br>


## Dataset

The table below summmarizes the structure of the dataset, which consists of approximately 10 Gigabytes of compressed files. 

| ***Group*** | ***Meshes*** | ***Cube*** | ***Tet*** | ***Pyramid*** | ***Octa*** | ***Star*** | ***Sphere*** | ***Polycube*** | ***Test pairs*** |
|:--------------:|:---------------:|:-------------:|:------------:|:----------------:|:-------------:|:-------------:|:---------------:|:-----------------:|:-------------------:|
| **G1**    | 25              | 25            | 24           | 25               | 24            | 25            | -               | -                 | 123                 |
| **G2**    | 67              | 67            | 66           | 66               | 67            | 67            | -               | -                 | 333                 |
| **G3**    | 1942            | 1932          | 1919         | 1926             | 1934          | 1924          | -               | -                 | 9635                |
| **G4**    | 2633            | 2607          | 2366         | 2460             | 2477          | 2611          | -               | -                 | 12521               |
| **G5**    | 26              | -             | -            | -                | -             | -             | 26              | -                 | 26                  |
| **G6**    | 4               | -             | -            | -                | -             | -             | -               | 4                 | 4                   |
| ***TOT***   | ***4697***            | ***4631***          | ***4375***         | ***4477***             | ***4502***          | ***4627***          | ***26***              | ***4***                 | ***22642***               |


To simplify download operations we have split the dataset into separated chunks, grouped w.r.t. the origin of the input models and the domain onto which the surface was mapped. Check Section 3 in the paper for more details about mesh sources, folder organization and file structures).

### Download links:


[**``[G1.zip]``**]() 
[**``[G1_cube.zip]``**]() 
[**``[G1_tet.zip]``**]() 
[**``[G1_pyr.zip]``**]() 
[**``[G1_octa.zip]``**]() 
[**``[G1_star.zip]``**]()

[**``[G2.zip]``**]() 
[**``[G2_cube.zip]``**]() 
[**``[G2_tet.zip]``**]() 
[**``[G2_pyr.zip]``**]() 
[**``[G2_octa.zip]``**]() 
[**``[G2_star.zip]``**]()

[**``[G3.zip]``**]() 
[**``[G3_cube.zip]``**]() 
[**``[G3_tet.zip]``**]() 
[**``[G3_pyr.zip]``**]() 
[**``[G3_octa.zip]``**]() 
[**``[G3_star.zip]``**]()

[**``[G4.zip]``**]() 
[**``[G4_cube.zip]``**]() 
[**``[G4_tet.zip]``**]() 
[**``[G4_pyr.zip]``**]() 
[**``[G4_octa.zip]``**]() 
[**``[G4_star.zip]``**]()

[**``[G5.zip]``**]() 
[**``[G5_sphere.zip]``**]()

[**``[G6.zip]``**]() 
[**``[G6_pc.zip]``**]()


## Auxiliary Code

VOLMAP includes a variety of facilities to ease the processing of the dataset, help assess the quality of its results and also extend it with additional models, maps and base domains (see Section 5.1 in the paper for more details). Each tool described below has its own *CMakeLists.txt* file, and uses the [Cinolib](https://github.com/mlivesu/cinolib) library as dependency. Compiling each tool (unless otherwise described) simply consists of opening a terminal inside the tool folder and typing:

```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -Dcinolib_DIR=<absolute-path-to-cinolib>
make
```

#### Tetmesh checks 
[This tool](./tetmesh_checks) ensures that a tetrahedral mesh *model.mesh* satisfies the VOLMAP requirements to compute the surface mapping: single connected component, genus zero, all positive volume elements, weel structured surface (i.e., manifold, closed and self-intersections free). The tool also checks the mesh tetrahedra according to the VOLMAP convention (see Section 5.1 in the paper). If the input mesh follows an opposite convention, this tool flips all its elements, saving a new file called *model_inv.mesh*. The syntax for using it is: 

```
./tetmesh_checks <filename.mesh>
```

It is possible to appen the check results in a *.csv* log file instead of printing them on screen. In this case the syntax becomes:

```
./tetmesh_checks <filename.mesh> <log_file.csv>
```

#### Surface mapping 
[This tool](./surface_mapping) implements the graph embedding strategy described in Section 5 in the paper, mapping a tetrahedram mesh in input into one of the VOLMAP base domains. The syntax for using it is: 

```
./surface_mapping_check <domain> <filename.mesh> <srf_mapping.txt>
```

where the ``<domain>`` param can be ``CUBE``, ``PYRAMID``, ``TETRAHEDRON``, ``OCTAHEDRON`` or ``STAR``. The computed per-vertex boundary conditions are stored in the ``<srf_mapping.txt>`` file, with the structure described in paragraph "*File Formats*" in Section 3 in the paper. After using this tool, use the [Surface mapping check tool](./surface_mapping_check) to verify that the generated mapping fulfills all the correctness requirements described in the paper.

The tool can also be used to map on simple custom domains not included in VOLMAP. To do so, users can provide an input file describing the abstract graph and its embedding. [Here](./surface_mapping/docs/custom_domain_instructions.md) you can find the instructions about how to construct this file.

#### Surface mapping check
[This tool](./surface_mapping_check) takes as input the original tetmesh and its boundary mapping computed with the previous tool and checks if it fulfills all the correctness requirements described in the paper (see "*Sanity check*" paragraph at the end of Section 5 for more details). The syntax for using it is: 

```
./surface_mapping_check <filename.mesh> <srf_mapping.txt>
```

#### Metrics
[This tool](./metrics) inputs the original tetmesh and its volume mapping computed with a candidate algorithm, and returns information regarding its validity and geometric distortion as described in the paper (see "*Metrics*" paragraph in Section 5 for more details). The syntax for using it is: 

```
./metrics <tetmesh.mesh> <vol_mapping.mesh>
```
Since the used metrics may tend to infinite, imposing an upper bound on the maximum value may be required. By default, we set the max value to 100, but you can change it with the following syntax:

```
./metrics <tetmesh.mesh> <vol_mapping.mesh> <upper_bound>
```

Finally, it is possible to specify a *.csv* log file in which to append the measured values. In this case, the syntax becomes:

```
./metrics <tetmesh.mesh> <vol_mapping.mesh> <upper_bound> <log_file.csv>
```

#### Map Visualization
[This tool](./map_visualization) can be used to visually inspect a volume mapping, plotting its inverted elements and color-coding tetrahdera according to some geometric distortion. All metrics described in the paper are supported and can be selected through the user interface (see "*Map visualization*" paragraph in Section 5 for more details). The syntax for using it is: 

```
./map_visualization <tetmesh.mesh> <vol_mapping.mesh>
```

#### Tetmeshing
[This tool](./tetmesh_generation) takes as input a surface mesh in *.OBJ* format and turn it into a tetrahedral mesh in *.MESH* format using [Tetgen](https://wias-berlin.de/software/index.jsp?id=TetGen&lang=1). After using this tool, use the [Tetmesh check tool](./tetmesh_check) to verify that the generated mesh satisfies the VOLMAP requirements. The syntax for using it is: 

```
./tetmesh_generation <srf_mesh.obj>
```

#### Mesh format converter
[This tool](./mesh_converter) allows you to convert a model from *.MESH* format to *.VTK* format and vice versa. Before compiling it, you need to install the [VTK](https://vtk.org/Wiki/VTK/Configure_and_Build) library. The syntax for using it is: 

``./mesh_convertes <filename.mesh>``  or  ``./mesh_convertes <filename.vtk>``


#### Python Scripts
In [this folder](./scripts), you can find several Python scripts to execute the aforementioned tools on large collections of shapes or to launch experiments with a given volume mapping algorithm. The description of each script is given as a comment at the top of each file.

## Cyting us

If you use our data or our code for your project, please cite our paper with the following BibTeX entry: 

```bibtex
@article{volmap2023,
  title   = {VOLMAP: a Large Scale Benchmark for Volume Mappings to Simple Base Domains},
  author  = {Cherchi, Gianmarco and Livesu, Marco},
  journal = {Computer Graphics Forum},
  year    = {2023},
  volume  = {},
  number  = {},
  pages   = {},
  doi     = {}
}
```
