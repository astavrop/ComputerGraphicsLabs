# Graphics Lab 05 — Phong Lighting, Procedural Checkerboard & Golden Bunny

An educational **C++ / OpenGL / GLSL** graphics experiment that develops a lighting model step by step, from a single triangle to a rotating, reflective **Stanford Bunny**.

> **Learning objective:** Understand how surface normals, lighting components, procedural patterns, and environment reflections affect the appearance of 3D geometry.

## Development journey

| Stage | Experiment | What we investigated |
| --- | --- | --- |
| 1 | **Single triangle** | The fundamental primitive used to construct polygonal meshes; introducing normals and lighting. |
| 2 | **3D sphere** | How lighting changes across a smoothly curved surface. |
| 3 | **Procedural checkerboard sphere** | Using GLSL calculations to generate a pattern without loading an image texture. |
| 4 | **Stanford Bunny** | Applying the lighting model to a more complex OBJ triangle mesh with surface normals. |
| 5 | **Golden Bunny + cubemap** | Improving the appearance of a gold-colored material by combining Phong lighting with direction-based environment sampling. |

## Phong lighting model

The fragment shader computes three independently controllable terms:

- **Ambient:** Baseline illumination, even when a surface does not directly face the light.
- **Diffuse:** Directional illumination based on the angle between the light direction and surface normal (`max(dot(N, L), 0.0)`).
- **Specular:** A view-dependent highlight, controlled by specular strength and **shininess**.

Conceptually:

```text
Phong color = ambient + diffuse + specular
```

The lab's **Dear ImGui** controls allow each lighting component to be enabled or disabled and its strength adjusted independently. A shininess control demonstrates how specular highlights become tighter as the exponent increases.

## Procedural checkerboard

A checkerboard pattern is generated directly in the fragment shader from local/object-space positions. This is **not UV image-texture mapping**: the pattern is calculated mathematically and stays attached to the object as it rotates.

The pattern helps make the interaction between surface geometry, diffuse illumination, and specular highlights easier to inspect.

## Stanford Bunny and gold material

The bunny mesh is loaded as triangles from an OBJ file, using vertex positions, normals, and indexed drawing. Rotating the model also requires transforming its normals consistently so the lighting remains correct.

The gold appearance was refined by tuning the Phong material and introducing **cubemap environment reflections**.

### Cubemap reflections

A cubemap stores six directional faces (`+X`, `-X`, `+Y`, `-Y`, `+Z`, `-Z`). For each visible bunny fragment, the shader computes a reflection direction from the view vector and surface normal, then samples the cubemap:

```glsl
vec3 N = normalize(vNormal);
vec3 I = normalize(FragPos - cameraPos);
vec3 R = reflect(I, N);
vec3 environmentColor = texture(skybox, R).rgb;
```

`FragPos`, `vNormal`, and `cameraPos` must be expressed in the same coordinate space. This is a fast **environment-mapping approximation**, not full ray tracing or physically based rendering.

A **six-color debug cubemap** can be generated procedurally to verify the mapping without downloading image assets. The final experiment blends the sampled environment color with Phong shading to improve the bunny's gold appearance.

## Interactive controls

| Control | Purpose |
| --- | --- |
| Ambient on/off + strength | Isolate and tune baseline illumination |
| Diffuse on/off + strength | Study normal–light orientation |
| Specular on/off + strength | Isolate and tune highlights |
| Shininess | Adjust highlight concentration |
| Checkerboard toggle / frequency | Compare plain material and procedural pattern |
| Cubemap reflections checkbox | Enable or disable environment mapping |
| Reflection strength | Blend environment reflections with Phong shading |

## Technologies and concepts

**C++, OpenGL, GLSL, GLFW, Dear ImGui, OBJ mesh loading, vertex normals, indexed triangle rendering, fragment shading, procedural patterns, reflection vectors, cubemap textures.**

## Running the lab

Build the **GraphicsLab05** project using the CMake configuration included in the repository. Ensure the OBJ asset path resolves relative to the executable's working directory (or configure an absolute/resource path as appropriate for your setup).

> Project layout and exact build target names may vary; consult the `CMakeLists.txt` in this folder.

## Results and report

The final demonstration is a **rotating golden Stanford Bunny** with independently adjustable Phong lighting and optional cubemap reflections.

If you add the PDF report to this folder, link it here, for example:

```markdown
[Read the illustrated Lab 05 report](Graphics_Lab05_Phong_Cubemap_Report.pdf)
```

## Next challenge: Texture mapping

With the gold appearance complete, the next lab will explore **UV coordinates, loading 2D image textures, and sampling them in GLSL** to control surface appearance.

---

Part of the [ComputerGraphicsLabs](https://github.com/astavrop/ComputerGraphicsLabs) educational, non-commercial graphics portfolio.
