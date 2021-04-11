# My EasyGL (Geometry is fun!)

## ToDO:
How to find the light direction given an image: n.dot(light) = diffuse
Can I add normal info when doing laplace inflation?

implement direct laplace smooth and implicit laplace smooth



## Skeleton Extraction by Mesh Contraction
Derive shape cost K by hand

### Implicit Laplace Smoothing
Let the vertex flow into mean-curvature normal.
```
du / dt = L * u
u(t+1) - u(t) = L * u(t+1)

so, solve (I-L)* u(t+1) = u(t)
```
Implementation
```
// M: mass matrix, Lcot: cotmatrix, L: laplace matrix, L = Minv * Lcot
// (I-L) * u(t+1) = u(t)
// (I-Minv*Lcot) * u(t+1) = u(t)
// (M - Lcot) * u(t+1) = u(t)
// so solve: (M - Lcot) * u(t+1) = u(t)
```
### How to dervie point-to-edge distance Matrix Kij?
ToDO: validate in code

## Q-MAT: Computing Medial Axis Transform by Quadratic Error Minimization

### Thinking
SkelByMeshContract (contract then collapse) == Q-MAT (find the optimal collapsed position to minimize reonstruct error)
## FAQ
#### 1. How to Check whether an edge is valid to be collapsed? is_collapse_ok in QEM

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

#### 4. How to understand laplace operator & efficiently implement it?
Flow-based understanding: divergence of flow. Mean-curvature normal.
* **Cotagent Laplace vs Uniform Laplace**: Cotagent Laplace is linear on plane.  (Lf_i)=0 for all interior vertices when positions of vertices are in the plane.

```
for f in faces:
    for v0, v1 in fes:
        L(v0, v0) -= cot(f, e01) # e01, edge between vertex 0 and vertex 1
        L(v1, v1) -= cot(f, e01)
        L(v0, v1) += cot(f, e01)
        L(v1, v0) += cot(f, e10)
```
#### 5. relationship between gradient operator G and laplace operator L
Laplace == divergence of gradient.
This is a visulization of [gradient field on triangle mesh](https://math.stackexchange.com/questions/3827267/gradient-descent-on-triangular-polyhedra-not-ml-how-to-prevent-jerks)

#### 6. Change parameter in Laplace Matrix == Editing Mesh Surface
#### 7. Why can't Curvture be defined on face, isn't it 90 rotation of normal?


## Video Link
https://www.bilibili.com/video/BV1B54y1B7Uc

## Design Principles and Code Style
* [Libigl](https://libigl.github.io/)
* Basic type such as vec, matrix must follow Eigen's API
## External Libraries

* [Eigen](http://eigen.tuxfamily.org/)

* https://github.com/USTC-GCL-F/AMMesh
