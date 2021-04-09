# A implementation of course "Digital Geometry Precessing" With ACAM Framework

## ToDO:
implement face-based style laplace operator and compare with halfedge-style (one-ring) based implementation

cal edge cot
for f in faces:
    for v0, v1 in fes:
        L(v0, v0) -= cot(f, e01) # e01, edge between vertex 0 and vertex 1
        L(v1, v1) -= cot(f, e01)
        L(v0, v1) += cot(f, e01)
        L(v1, v0) += cot(f, e10)
## Skeleton Extraction by Mesh Contraction

## Q-MAT: Computing Medial Axis Transform by Quadratic Error Minimization

### Thinking
SkelByMeshContract (contract then collapse) == Q-MAT (find the optimal collapsed position to minimize reonstruct error)
## FAQ
#### 1. How to Check an edge is valid to be collapsed? is_collapse_ok in QEM

#### 2. addPolyFace patch_relink?

#### 3. How to efficiently compute local average region for triangle mesh?

Refer hw2
```
Input: mesh (halfedge datastructure)
Output: vUmbrella (local average region, one-ring area)

# mixed voronoi: edge midpoint for obtuse triangle
for f in faces:
    if f.isObtuse:
       for v in fvs:
           if v.isObtuse:
              vUmbrella[v] += 0.5 * fArea
           else:
              vUmbrella[v] += 0.25 * fArea
    else:
        cent = f.circumcenter()
        for v0, v1 in fedgepoints:
            midp = (v0 + v1) / 2
            half_len = 0.5 * (v0-v1).norm()
            areaRightAngTri = (midp-cent).norm() * half_len) /2
            vUmbrella[v0] += 0.5 * areaRightAngTri # !!! multiply 0.5 since each vertex is counted twice
            vUmbrella[v1] += 0.5 * areaRightAngTri # !!! multiply 0.5 since each vertex is counted twice
        

# barycentric: barycenter, edgemidpoints
for f in faces:
    for v0, v1 in fedgepoints:
        vTri = f.area() / 6 # barycenter and edge midpoint split the whole triangle face into 6 equal-size triangles
        vUmbrella[v0] += 0.5 * vTri # !!! multiply 0.5 since each vertex is counted twice
        vUmbrella[v1] += 0.5 * vTri # !!! multiply 0.5 since each vertex is counted twice
```

#### 4. relationship between gradient operator G and laplace operator L
This is a visulization of [gradient field on triangle mesh](https://math.stackexchange.com/questions/3827267/gradient-descent-on-triangular-polyhedra-not-ml-how-to-prevent-jerks)

laplace == divergence of gradient


#### 5. How to compute laplace operator and gradient operator?
## Video Link
https://www.bilibili.com/video/BV1B54y1B7Uc

## External Libraries

* [Eigen](http://eigen.tuxfamily.org/)

## Usage
https://github.com/USTC-GCL-F/AMMesh
