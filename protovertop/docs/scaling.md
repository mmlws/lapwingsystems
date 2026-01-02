# Variable Scaling

### Polynomial Coefficients
Polynomial is defined as 
$$ P(s) = a_0 + s a_1  + s^2 a_2 $$

Take $a_0$ reference scale as 1. For $a_1$ and $a_2$, we want the same change in magnitude in $P(s)$ 
for the same *scaled* change.  Define internal variables $\hat{a}_1$ and $\hat{a}_2$ with scaling coefficients
$\alpha_1$ and $\alpha_2$:
$$
\begin{align}
    a_1 &= \alpha_1 \hat{a}_1 \\ 
    a_2 &= \alpha_2 \hat{a}_2
\end{align}
$$

We want $\frac{dP}{d \hat{a}_i} = 1$:
$$
    \frac{dP}{d\hat{a}_1} = \frac{dP}{da_1}\frac{da_1}{d\hat{a}_1} = s\alpha_1 = 1 \rightarrow \alpha_1 = 1/s
$$
$$
    \frac{dP}{d\hat{a}_2} = \frac{dP}{da_2}\frac{da_2}{d\hat{a}_2} = s^2\alpha_2 = 1 \rightarrow \alpha_2 = 1/s^2
$$

Since most constraints are taken at segment ends, it makes sense to use $\Delta s$ which is the typical segment length
$$
\begin{align}
    \alpha_1 &= \frac{1}{\Delta s} \\ 
    \alpha_2 &= \frac{1}{\Delta s^2}
\end{align}
$$

### Offsets and Slack Variables
Variables $u$, $u^\text{fill}$ and $u^\text{cut}$ change similarly to $a_0$ due to gap constraint:
$$
    \begin{align}
        P(s_i) - u_i &= Z_i \\
        a_0 + s a_1 + s^2 a_2 - u_i &= Z_i
    \end{align}
$$

### Flow and Volume Variables
Flow and volume variables have the same scale as they are linked directly by the balance constraints:
$$
    \begin{align} 
        V^\text{fill} &= f^{u+} + f^{u-} \\
        V^\text{cut} &= f^{l+} + f^{l-} 
    \end{align}
$$

To get the correct scaling, we need to look at the volume constraint
$$
    C = V - au^2 - bu - c \geq 0
$$

Take the scaled variables as $V = k_V \hat{V}$ and $u = k_u \hat{u}$.  We want the magnitude change in $C$
to be the same for unit changes in $\hat{V}$ and $\hat{u}$:
$$
    \begin{align}
        \frac{dC}{d\hat{V}} &= k_V \\
        \frac{dC}{d\hat{u}} &= (-2au - b) k_u
    \end{align}
$$
Noting that $k_u = 1$ and $k_V > 0$ we have 
$$
    \begin{align}
        k_V &= |2au + b| \\
        k_V &= \text{max}\left(|2au_\text{max} + b|, |2au_\text{min} + b|\right)
    \end{align}
$$