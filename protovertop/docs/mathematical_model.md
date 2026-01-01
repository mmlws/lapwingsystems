# Vertical Alignment Optimization: Mathematical Model

**Single Road, Single Material, Single Haul Path**

*Based on: Sadhukhan, Hare, Lucet (2025) - "A unified vertical alignment and earthwork model in road design"*

---

## 1. Problem Overview

Given a horizontal road alignment with known ground elevation profile, find the optimal vertical road profile that minimizes earthwork cost while satisfying safety and design constraints.

**Key Simplifications:**
- Single road (no network/intersections)
- Single material type
- Single haul path (one equipment type)
- Stations coincide with segment boundaries

---

## 2. Spatial Discretization

### 2.1 Segments and Stations

The road is divided into **segments** indexed by $g \in \{0, 1, \ldots, G-1\}$ where $G$ is the number of segments.

**Stations** are the segment boundary points indexed by $i \in \{0, 1, \ldots, G\}$ (there are $G+1$ stations for $G$ segments).

- Station $i$ is located at position $s_i$ (in meters along the road)
- Segment $g$ spans from station $g$ to station $g+1$, i.e., $[s_g, s_{g+1}]$
- Segment length: $L_g = s_{g+1} - s_g$

**Example:**
```
Stations:  s₀=0    s₁=20   s₂=40   s₃=60  (4 stations)
Segments:     |--g=0--|--g=1--|--g=2--|     (3 segments)
```

---

## 3. Road Profile Representation

### 3.1 Piecewise Quadratic Spline

The vertical road elevation is represented as a **piecewise quadratic spline**. For each segment $g$, the elevation at position $s \in [s_g, s_{g+1}]$ is:

$$
P_g(s) = a_{0,g} + a_{1,g}(s - s_g) + a_{2,g}(s - s_g)^2
$$

where:
- $a_{0,g}$ = elevation at the start of segment $g$ (constant term)
- $a_{1,g}$ = initial gradient of segment $g$ (linear term)
- $a_{2,g}$ = curvature of segment $g$ (quadratic term)

### 3.2 Gradient (Derivative)

The road gradient (slope) at position $s$ in segment $g$ is:

$$
P_g'(s) = a_{1,g} + 2a_{2,g}(s - s_g)
$$

### 3.3 Second Derivative (Curvature)

The rate of change of gradient (constant within each segment):

$$
P_g''(s) = 2a_{2,g}
$$

---

## 4. Decision Variables

### 4.1 Geometric Variables (Primary)

For each segment $g \in \{0, 1, \ldots, G-1\}$:
- $a_{0,g}$ - elevation at the start of segment g (constant term)
- $a_{1,g}$ - initial gradient of segment g (linear term)
- $a_{2,g}$ - curvature of segment g (quadratic term)

**Total geometric variables:** $3G$

### 4.2 Volume Variables

For each segment $g \in \{0, 1, \ldots, G-1\}$:
- $V_g^\text{cut} \geq 0$ - cut volume (excavation) in m³
- $V_g^\text{fill} \geq 0$ - fill volume (embankment) in m³

**Total volume variables:** $2G$

### 4.3 Flow Variables

The quasi-network flow model requires several flow types at each segment to properly model material movement:

For each segment $g \in \{0, 1, \ldots, G-1\}$:

**Transit Flows** (material passing through):
- $f_g^{t+} \geq 0$ - transit flow in forward direction (left to right)
- $f_g^{t-} \geq 0$ - transit flow in backward direction (right to left)

**Loading Flows** (material extracted from segment):
- $f_g^{l+} \geq 0$ - loading flow to forward direction
- $f_g^{l-} \geq 0$ - loading flow to backward direction

**Unloading Flows** (material deposited to segment):
- $f_g^{u+} \geq 0$ - unloading flow from forward direction
- $f_g^{u-} \geq 0$ - unloading flow from backward direction

**Borrow Pit Flows** (for segments with borrow pit access):
- $f_g^{b+} \geq 0$ - material from borrow pit to forward direction
- $f_g^{b-} \geq 0$ - material from borrow pit to backward direction

**Waste Pit Flows** (for segments with waste pit access):
- $f_g^{w+} \geq 0$ - material to waste pit from forward direction
- $f_g^{w-} \geq 0$ - material to waste pit from backward direction

**Note:** For simplified case without borrow/waste pits, the last four flow types are omitted.

**Total flow variables:**
- Without borrow/waste: $6G$
- With borrow/waste: $10G$ (assuming one pit per segment)

**Total decision variables:**
- Without borrow/waste: $3G + 2G + 6G = 11G$
- With borrow/waste: $3G + 2G + 10G = 15G$

---

## 5. Ground Profile

The existing ground elevation at station $i$ is denoted $Z_i$ (given data).

For positions between stations, ground elevation can be:
- Linearly interpolated: $Z(s) = Z_g + (s - s_g)\frac{Z_{g+1} - Z_g}{L_g}$ for $s \in [s_g, s_{g+1}]$
- Or defined by a more complex terrain model

---

## 6. Offset (Gap)

The **offset** at station $i$ is the difference between road elevation and ground elevation:

$$
u_i = P_g(s_i) - Z_i
$$

where $g$ is the segment that ends at station $i$.

**Sign convention:**
- $u_i > 0$ → road is above ground (fill required)
- $u_i < 0$ → road is below ground (cut required)
- $u_i = 0$ → road matches ground (no earthwork)

---

## 7. Constraints

### 7.1 Continuity Constraints

**C⁰ Continuity (Elevation Continuity):**

At each interior station $i \in \{1, 2, \ldots, G-1\}$, the road elevation must be continuous:

$$
P_{i-1}(s_i) = P_i(s_i)
$$

Expanding using the quadratic form:

$$
a_{0,i-1} + a_{1,i-1}L_{i-1} + a_{2,i-1}(L_{i-1})^2 = a_{0,i}
$$

**C¹ Continuity (Gradient Continuity):**

At each interior station $i \in \{1, 2, \ldots, G-1\}$, the road gradient must be continuous:

$$
P_{i-1}'(s_i) = P_i'(s_i)
$$

Expanding:

$$
a_{1,i-1} + 2a_{2,i-1} L_{i-1} = a_{1,i}
$$

**Total continuity constraints:** $2(G-1)$ linear equations

### 7.2 Grade (Gradient) Constraints

The road gradient must satisfy safety bounds at all stations:

$$
G_{\min} \leq P_g'(s_i) \leq G_{\max} \quad \text{for all stations } i
$$

where:
- $G_{\min}$ = minimum allowable grade (e.g., -0.08 for -8%)
- $G_{\max}$ = maximum allowable grade (e.g., +0.08 for +8%)

At the start of each segment $g$:
$$
G_{\min} \leq a_{1,g} \leq G_{\max}
$$

At the end of each segment $g$:
$$
G_{\min} \leq a_{1,g} + 2a_{2,g} \cdot L_g \leq G_{\max}
$$

**Total grade constraints:** $4G$ linear inequalities (2 per segment)

### 7.3 Offset Constraints

The offset must satisfy design bounds at each station:

$$
u_{\min} \leq u_i \leq u_{\max} \quad \text{for all stations } i \in \{0, 1, \ldots, G\}
$$

where:
- $u_{\min} < 0$ = maximum allowable cut depth
- $u_{\max} > 0$ = maximum allowable fill height

These bounds define the vertical corridor within which the road can be placed.

Expressing in terms of decision variables:
$$
u_{\min} \leq P_g(s_i) - Z_i \leq u_{\max}
$$

For station $i$ at the end of segment $i-1$:
$$
u_{\min} \leq a_{0,i-1} + a_{1,i-1} \cdot L_{i-1} + a_{2,i-1} \cdot (L_{i-1})^2 - Z_i \leq u_{\max}
$$

**Total offset constraints:** $2(G+1)$ linear inequalities

### 7.4 Volume Constraints

The cross-sectional area with side slopes is a function of offset. Volume for segment $g$ (spanning stations $g$ to $g+1$) depends on offsets $u_g$ and $u_{g+1}$ at the segment endpoints.

**Two formulations are available:**

#### 7.4.1 Linear Volume Model

For segment $g$ with offsets $u_g$ and $u_{g+1}$:

**Cut volume:**
$$
V_g^+ \geq \chi_{0,g}^+ + \chi_{1,g}^+ u_g + \chi_{2,g}^+ u_{g+1}
$$

**Fill volume:**
$$
V_g^- \geq \chi_{0,g}^- + \chi_{1,g}^- u_g + \chi_{2,g}^- u_{g+1}
$$

where $\chi_{0,g}, \chi_{1,g}, \chi_{2,g}$ are precomputed coefficients based on ground profile and road width.

**Note:** These are **linear constraints**.

#### 7.4.2 Quadratic Volume Model (CUVA)

Using the **CUVA convex approximation** (Equations 40-41 from paper), volumes are approximated using least-squares fitted quadratic functions of station offsets:

**Cut volume:**
$$
V_g^+ \geq \chi_{0,g}^+ + \chi_{1,g}^+ u_g + \chi_{2,g}^+ u_{g+1} + \chi_{3,g}^+ u_g^2 + \chi_{4,g}^+ u_g u_{g+1} + \chi_{5,g}^+ u_{g+1}^2
$$

**Fill volume:**
$$
V_g^- \geq \chi_{0,g}^- + \chi_{1,g}^- u_g + \chi_{2,g}^- u_{g+1} + \chi_{3,g}^- u_g^2 + \chi_{4,g}^- u_g u_{g+1} + \chi_{5,g}^- u_{g+1}^2
$$

where:
- $u_g = P_{g-1}(s_g) - Z_g$ = offset at left station (if $g > 0$) or $u_g = a_{0,0} - Z_0$ (if $g = 0$)
- $u_{g+1} = P_g(s_{g+1}) - Z_{g+1} = a_{0,g} + a_{1,g} L_g + a_{2,g} L_g^2 - Z_{g+1}$
- $\chi_{0,g}, \ldots, \chi_{5,g}$ = least-squares fitted coefficients (precomputed from ground data and cross-section geometry)

**Note:** These are **convex quadratic constraints** (inequality form with $\geq$, positive semidefinite Hessian).

**Total volume constraints:**
- Linear model: $2G$ linear inequalities
- Quadratic model (CUVA): $2G$ convex quadratic inequalities

### 7.5 Flow Balance Constraints

The quasi-network flow model tracks material movement through loading, transit, and unloading operations.

**Material Balance at Segment** $g \in \{0, 1, \ldots, G-1\}$:

$$
V_g^+ + f_g^{u+} + f_g^{u-} + f_g^{b+} + f_g^{b-} = V_g^- + f_g^{l+} + f_g^{l-} + f_g^{w+} + f_g^{w-}
$$

Interpretation:
- **Left side (material available):**
  - $V_g^+$ = material excavated from segment $g$
  - $f_g^{u+}$ = material unloaded from forward transit
  - $f_g^{u-}$ = material unloaded from backward transit
  - $f_g^{b+}$ = material from borrow pit to forward direction
  - $f_g^{b-}$ = material from borrow pit to backward direction

- **Right side (material consumed):**
  - $V_g^-$ = material placed as fill in segment $g$
  - $f_g^{l+}$ = material loaded to forward transit
  - $f_g^{l-}$ = material loaded to backward transit
  - $f_g^{w+}$ = material to waste pit from forward direction
  - $f_g^{w-}$ = material to waste pit from backward direction

**Transit Flow Continuity:**

Forward direction at segment boundary between $g$ and $g+1$:

$$
f_g^{t+} = f_{g-1}^{t+} + f_g^{l+} + f_g^{b+} - f_g^{u+} - f_g^{w+}
$$

Backward direction at segment boundary between $g$ and $g+1$:

$$
f_{g-1}^{t-} = f_g^{t-} + f_g^{l-} + f_g^{b-} - f_g^{u-} - f_g^{w-}
$$

Interpretation:
- Transit flow out = transit flow in + loading + borrow - unloading - waste

**Simplified Case (no borrow/waste pits):**

Material balance becomes:
$$
V_g^+ + f_g^{u+} + f_g^{u-} = V_g^- + f_g^{l+} + f_g^{l-}
$$

Transit flow continuity:
$$
f_g^{t+} = f_{g-1}^{t+} + f_g^{l+} - f_g^{u+}
$$
$$
f_{g-1}^{t-} = f_g^{t-} + f_g^{l-} - f_g^{u-}
$$

**Boundary Conditions:**

At segment $g = 0$ (leftmost):
$$
f_{-1}^{t+} = 0 \quad \text{(no transit arriving from left)}
$$

At segment $g = G-1$ (rightmost):
$$
f_{G-1}^{t+} = 0 \quad \text{(no transit departing to right)}
$$
$$
f_{G-1}^{t-} = 0 \quad \text{(no transit arriving from right)}
$$

**Total balance constraints:**
- Without borrow/waste: $G$ (material balance) + $2(G-1)$ (transit continuity) = $3G - 2$ linear equations
- With borrow/waste: $G$ (material balance) + $2(G-1)$ (transit continuity) = $3G - 2$ linear equations

### 7.6 Non-Negativity Constraints

**Volume variables:**
$$
V_g^+ \geq 0, \quad V_g^- \geq 0 \quad \text{for all } g \in \{0, \ldots, G-1\}
$$

**Flow variables (without borrow/waste):**
$$
f_g^{t+} \geq 0, \quad f_g^{t-} \geq 0 \quad \text{for all } g \in \{0, \ldots, G-2\}
$$
$$
f_g^{l+} \geq 0, \quad f_g^{l-} \geq 0 \quad \text{for all } g \in \{0, \ldots, G-1\}
$$
$$
f_g^{u+} \geq 0, \quad f_g^{u-} \geq 0 \quad \text{for all } g \in \{0, \ldots, G-1\}
$$

**Additional flow variables (with borrow/waste pits):**
$$
f_g^{b+} \geq 0, \quad f_g^{b-} \geq 0 \quad \text{for segments with borrow pit access}
$$
$$
f_g^{w+} \geq 0, \quad f_g^{w-} \geq 0 \quad \text{for segments with waste pit access}
$$

### 7.7 Borrow and Waste Pit Capacity Constraints

Borrow and waste pits have finite capacities that must be respected.

**Borrow Pit Capacity:**

For each borrow pit $k$ with maximum capacity $B_k^{\max}$ (in m³), the total material extracted cannot exceed the capacity:

$$
\sum_{g \in \mathcal{B}_k} (f_g^{b+} + f_g^{b-}) \leq B_k^{\max}
$$

where $\mathcal{B}_k \subseteq \{0, 1, \ldots, G-1\}$ is the set of segments that have access to borrow pit $k$.

**Waste Pit Capacity:**

For each waste pit $j$ with maximum capacity $W_j^{\max}$ (in m³), the total material deposited cannot exceed the capacity:

$$
\sum_{g \in \mathcal{W}_j} (f_g^{w+} + f_g^{w-}) \leq W_j^{\max}
$$

where $\mathcal{W}_j \subseteq \{0, 1, \ldots, G-1\}$ is the set of segments that have access to waste pit $j$.

**Simplified case (single borrow pit, single waste pit):**

If there is only one borrow pit accessible from all segments in $\mathcal{B}$:
$$
\sum_{g \in \mathcal{B}} (f_g^{b+} + f_g^{b-}) \leq B^{\max}
$$

If there is only one waste pit accessible from all segments in $\mathcal{W}$:
$$
\sum_{g \in \mathcal{W}} (f_g^{w+} + f_g^{w-}) \leq W^{\max}
$$

**Total capacity constraints:**
- Number of borrow pit constraints: $|\text{borrow pits}|$
- Number of waste pit constraints: $|\text{waste pits}|$

---

## 8. Objective Function

Minimize the total earthwork cost:

$$
C_{\text{total}} = C_{\text{cut}} + C_{\text{fill}} + C_{\text{load}} + C_{\text{haul}} + C_{\text{unload}} + C_{\text{borrow}} + C_{\text{waste}}
$$

### 8.1 In-Situ Operations

**Cut (Excavation) Cost:**
$$
C_{\text{cut}} = p \sum_{g=0}^{G-1} V_g^+
$$

**Fill (Compaction) Cost:**
$$
C_{\text{fill}} = q \sum_{g=0}^{G-1} V_g^-
$$

where:
- $p$ = unit cost of excavation in-place ($/m³)
- $q$ = unit cost of placement and compaction ($/m³)

### 8.2 Material Handling Costs

**Loading Cost:**
$$
C_{\text{load}} = y_l \sum_{g=0}^{G-1} (f_g^{l+} + f_g^{l-})
$$

**Haul (Transport) Cost:**
$$
C_{\text{haul}} = c \sum_{g=0}^{G-2} L_g \cdot (f_g^{t+} + f_g^{t-})
$$

**Unloading Cost:**
$$
C_{\text{unload}} = y_u \sum_{g=0}^{G-1} (f_g^{u+} + f_g^{u-})
$$

where:
- $y_l$ = unit cost of loading material onto trucks ($/m³)
- $c$ = unit cost of hauling per unit distance ($/m³·m)
- $y_u$ = unit cost of unloading material from trucks ($/m³)
- $L_g$ = segment length (haul distance)

**Note:** Often $y_l = y_u = y$ (same cost for loading and unloading).

### 8.3 External Sources and Disposal

**Borrow Pit Cost:**
$$
C_{\text{borrow}} = b \sum_{g \in \mathcal{B}} (f_g^{b+} + f_g^{b-})
$$

**Waste Disposal Cost:**
$$
C_{\text{waste}} = w \sum_{g \in \mathcal{W}} (f_g^{w+} + f_g^{w-})
$$

where:
- $b$ = unit cost of borrow material, including extraction and loading ($/m³)
- $w$ = unit cost of waste disposal ($/m³)
- $\mathcal{B}$ = set of segments with borrow pit access
- $\mathcal{W}$ = set of segments with waste pit access

### 8.4 Combined Objective Function

**Simplified case (no borrow/waste, $y_l = y_u = y$):**
$$
\text{minimize: } p \sum_{g=0}^{G-1} V_g^+ + q \sum_{g=0}^{G-1} V_g^- + y \sum_{g=0}^{G-1} (f_g^{l+} + f_g^{l-} + f_g^{u+} + f_g^{u-}) + c \sum_{g=0}^{G-2} L_g (f_g^{t+} + f_g^{t-})
$$

**General case (with borrow/waste, separate loading/unloading costs):**
$$
\begin{align}
\text{minimize: } \quad & p \sum_{g=0}^{G-1} V_g^+ + q \sum_{g=0}^{G-1} V_g^- \\
& + y_l \sum_{g=0}^{G-1} (f_g^{l+} + f_g^{l-}) + y_u \sum_{g=0}^{G-1} (f_g^{u+} + f_g^{u-}) \\
& + c \sum_{g=0}^{G-2} L_g (f_g^{t+} + f_g^{t-}) \\
& + b \sum_{g \in \mathcal{B}} (f_g^{b+} + f_g^{b-}) + w \sum_{g \in \mathcal{W}} (f_g^{w+} + f_g^{w-})
\end{align}
$$

---

## 9. Complete Optimization Model

### 9.1 Model Type

**Convex Quadratically-Constrained Quadratic Program (QCQP)** or **Quadratic Program (QP)**

#### Linear Volume Model

**Without borrow/waste:**
- Decision variables: $11G$ (continuous, real-valued)
  - Geometric: $3G$
  - Volume: $2G$
  - Flow: $6G$
- Linear equality constraints: $5G - 4$
  - Continuity (C⁰ and C¹): $2(G-1)$
  - Flow balance and transit continuity: $3G - 2$
- Linear inequality constraints: $6G + 2 + 2G = 8G + 2$
  - Grade bounds: $4G$
  - Offset bounds: $2(G+1)$
  - Volume constraints: $2G$
- Objective: Linear

**Model type:** Quadratic Program (QP) - polynomial time solvable

**With borrow/waste (K borrow pits, J waste pits):**
- Decision variables: $15G$ (adds $4G$ for borrow/waste flows)
- Linear equality constraints: $5G - 4$ (same as above)
- Linear inequality constraints: $8G + 2 + K + J$
  - Same as above: $8G + 2$
  - Borrow pit capacities: $K$
  - Waste pit capacities: $J$
- Objective: Linear

**Model type:** Quadratic Program (QP)

#### Quadratic Volume Model (CUVA)

**Without borrow/waste:**
- Decision variables: $11G$ (same as linear model)
- Linear equality constraints: $5G - 4$ (same as linear model)
- Linear inequality constraints: $6G + 2$
  - Grade bounds: $4G$
  - Offset bounds: $2(G+1)$
- Quadratic constraints: $2G$ (convex volume constraints)
- Objective: Linear

**Model type:** Convex QCQP - polynomial time solvable

**With borrow/waste (K borrow pits, J waste pits):**
- Decision variables: $15G$
- Linear equality constraints: $5G - 4$
- Linear inequality constraints: $6G + 2 + K + J$
  - Grade and offset bounds: $6G + 2$
  - Borrow pit capacities: $K$
  - Waste pit capacities: $J$
- Quadratic constraints: $2G$ (convex)
- Objective: Linear

**Model type:** Convex QCQP

### 9.2 Standard Form

$$
\begin{align}
\text{minimize} \quad & c^T x \\
\text{subject to} \quad & Ax = b \quad \text{(continuity, balance, transit continuity)} \\
& Bx \leq d \quad \text{(grade, offset bounds)} \\
& x^T Q_g x + q_g^T x + r_g \leq 0 \quad \text{(volume, } g=0 \ldots G-1\text{)} \\
& x \geq 0 \quad \text{(non-negativity on volumes and flows)}
\end{align}
$$

**Decision vector (without borrow/waste):**

$$
\begin{align}
x = [&a_{0,0}, a_{1,0}, a_{2,0}, \ldots, a_{0,G-1}, a_{1,G-1}, a_{2,G-1}, \\
&V_0^+, \ldots, V_{G-1}^+, V_0^-, \ldots, V_{G-1}^-, \\
&f_0^{t+}, \ldots, f_{G-2}^{t+}, f_0^{t-}, \ldots, f_{G-2}^{t-}, \\
&f_0^{l+}, \ldots, f_{G-1}^{l+}, f_0^{l-}, \ldots, f_{G-1}^{l-}, \\
&f_0^{u+}, \ldots, f_{G-1}^{u+}, f_0^{u-}, \ldots, f_{G-1}^{u-}]
\end{align}
$$

**With borrow/waste, append:**
$$
[f_0^{b+}, \ldots, f_{G-1}^{b+}, f_0^{b-}, \ldots, f_{G-1}^{b-}, f_0^{w+}, \ldots, f_{G-1}^{w+}, f_0^{w-}, \ldots, f_{G-1}^{w-}]
$$

### 9.3 Solution Method

- **Convex optimization:** Global optimum guaranteed
- **Solvers:** CPLEX, Gurobi, MOSEK (commercial), ECOS, SCS (open-source)
- **Complexity:** Polynomial time in number of variables and constraints

---

## 10. Key Properties

### 10.1 Convexity

The model is **convex** because:
1. Objective function is linear (convex)
2. Equality constraints are linear (convex)
3. Inequality constraints are linear or convex quadratic
4. Volume constraints use $\geq$ form with positive-definite Hessian

### 10.2 Scalability

For a road with:
- Length: $L$ meters
- Segment spacing: $\Delta s$ meters
- Number of segments: $G = L/\Delta s$

**Example:** 1000m road, 20m segments → $G = 50$

#### Linear Volume Model (QP)

**Without borrow/waste:**
- Variables: $11(50) = 550$
- Linear equality constraints: $5(50) - 4 = 246$
- Linear inequality constraints: $8(50) + 2 = 402$

**With borrow/waste (1 borrow pit, 1 waste pit):**
- Variables: $15(50) = 750$
- Linear equality constraints: $246$ (same)
- Linear inequality constraints: $402 + 1 + 1 = 404$

**Solution time:** Modern QP solvers (Gurobi, CPLEX, MOSEK) solve in **< 1 second**

#### Quadratic Volume Model (CUVA - QCQP)

**Without borrow/waste:**
- Variables: $11(50) = 550$
- Linear equality constraints: $5(50) - 4 = 246$
- Linear inequality constraints: $6(50) + 2 = 302$
- Quadratic constraints: $2(50) = 100$

**With borrow/waste (1 borrow pit, 1 waste pit):**
- Variables: $15(50) = 750$
- Linear equality constraints: $246$ (same)
- Linear inequality constraints: $302 + 1 + 1 = 304$
- Quadratic constraints: $100$ (same)

**Solution time:** Modern convex QCQP solvers (MOSEK, Gurobi, CPLEX) solve in **seconds** to **minutes**

**Note:** The quadratic model (CUVA) provides more accurate volume estimates but requires slightly more computation time than the linear model.

---

## 11. Notation Summary

### Spatial Discretization
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $G$                         | Number of segments                | -                           |
| $g$                         | Segment index                     | $\in \{0, 1, \ldots, G-1\}$ |
| $i$                         | Station index                     | $\in \{0, 1, \ldots, G\}$   |
| $s_i$                       | Station position                  | m                           |
| $L_g$                       | Segment length                    | m                           |

### Road Profile
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $P_g(s)$                    | Road elevation in segment g       | m                           |
| $a_{0,g}, a_{1,g}, a_{2,g}$ | Quadratic coefficients            | m, -, m⁻¹                   |
| $Z_i$                       | Ground elevation at station i     | m                           |
| $u_i$                       | Offset at station i               | m                           |

### Decision Variables - Volumes
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $V_g^+$                     | Cut volume in segment g           | m³                          |
| $V_g^-$                     | Fill volume in segment g          | m³                          |

### Decision Variables - Flows
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $f_g^{t+}, f_g^{t-}$        | Transit flow (forward/backward)   | m³                          |
| $f_g^{l+}, f_g^{l-}$        | Loading flow (forward/backward)   | m³                          |
| $f_g^{u+}, f_g^{u-}$        | Unloading flow (forward/backward) | m³                          |
| $f_g^{b+}, f_g^{b-}$        | Borrow pit flow (forward/backward)| m³                          |
| $f_g^{w+}, f_g^{w-}$        | Waste pit flow (forward/backward) | m³                          |

### Constraint Bounds
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $G_{\min}, G_{\max}$        | Grade bounds                      | - (or %)                    |
| $u_{\min}, u_{\max}$        | Offset bounds                     | m                           |
| $B_k^{\max}$                | Maximum capacity of borrow pit k  | m³                          |
| $W_j^{\max}$                | Maximum capacity of waste pit j   | m³                          |

### Volume Approximation Coefficients
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $\chi_{0,g}^+, \ldots, \chi_{5,g}^+$ | Cut volume coefficients (quadratic model) | various      |
| $\chi_{0,g}^-, \ldots, \chi_{5,g}^-$ | Fill volume coefficients (quadratic model) | various     |

### Cost Coefficients
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $p$                         | Unit excavation cost              | $/m³                        |
| $q$                         | Unit fill/compaction cost         | $/m³                        |
| $y_l$                       | Unit loading cost                 | $/m³                        |
| $y_u$                       | Unit unloading cost               | $/m³                        |
| $y$                         | Unit loading/unloading cost (when equal) | $/m³                 |
| $c$                         | Unit haul cost                    | $/m³·m                      |
| $b$                         | Unit borrow cost (incl. extraction) | $/m³                      |
| $w$                         | Unit waste disposal cost          | $/m³                        |

### Sets
| Symbol                      | Description                       | Units                       |
|-----------------------------|-----------------------------------|-----------------------------|
| $\mathcal{B}$               | Set of segments with borrow pit access | -                      |
| $\mathcal{W}$               | Set of segments with waste pit access | -                       |
| $\mathcal{B}_k$             | Set of segments accessing borrow pit k | -                      |
| $\mathcal{W}_j$             | Set of segments accessing waste pit j | -                       |

---

## 12. References

1. Sadhukhan, S., Hare, W., & Lucet, Y. (2025). A unified vertical alignment and earthwork model in road design with a new convex optimization model for road networks. *Engineering Optimization*.

2. Momo, N. S., Hare, W., & Lucet, Y. (2023). Modeling side slopes in vertical alignment resource road construction using convex optimization. *Computer-Aided Civil and Infrastructure Engineering*, 38(2), 211-224.

3. Hare, W., Lucet, Y., & Rahman, F. (2015). A mixed-integer linear programming model to optimize the vertical alignment considering blocks and side-slopes in road construction. *European Journal of Operational Research*, 241(3), 631-641.
