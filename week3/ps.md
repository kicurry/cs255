1. Let $(S, V)$ be a secure MAC deifined over $(K, M, T)$ where $M = K = \{0, 1\}^n$ and $T = \{0, 1\}^{128}$

   - [x] $S'(k, m) = S(k, m)[0,\dots,126]$ and $V'(k,m,t)=[V(k,m,t||0) \text{ or } V(k,m,t||1)]$

     The lecture shows that we can *truncate* **MACs based on PRFs**. And this choice shows that not only MACs based on PRFs hold, but also all other MACs.(But I haven't try to proof this, just according to the answer of Problem Set)

2. Let $H: M \to T$ be a random hash function where $|M|\gg|T|$(i.e. the size of $M$ is much larger than the size of $T$). How many random samples would it take until we obtain a three way collision, namely distinct strings $x,y,z$ in $M$ such that $H(x) = H(y) = H(z)$?

   Recall "Birthday Paradox", we can use "Union Bound" to get
   $$
   \begin{equation}
   \begin{split}
   \text{Pr}[\text{2-match}] &= \text{Pr}[\bigcup_{i = 1}^{\text{C}_n^2} \text P_i] \\
   &\le\sum_{i = 1}^{\text{C}_n^2}\text{Pr}[\text P_i]
   \end{split}
   \end{equation}
   $$
   where $\text P_i$ means pair-i$(x_i, y_i)$ matches(i.e. $H(x_i)=H(y_i)$) and $\text{Pr}[\text P_i] = \frac 1 {|T|}$(Fix $x_i$, only the case where $H(y_i) = H(x_i)$ in $T$ space satisfys). Then, we get
   $$
   \text{Pr}[\text{2-match}] 
   \le\text{C}_n^2\cdot\text{Pr}[\text P_i] = \frac{n(n-1)}{2|T|}\le \frac{n^2}{2|T|}
   $$
   Likely, let $\text P_i$ be the event when triple-i$(x_i, y_i,z_i)$ matches. Obviously, $\text{Pr}[\text P_i] = \frac 1 {|T|^2}$. Then, we get
   $$
   \text{Pr}[\text{3-match}] 
   \le\text{C}_n^3\cdot\text{Pr}[\text P_i] \le \frac{n^3}{6|T|^2}\approx \frac 1 2
   $$
   Therefore, we can find three-way collision w.h.p when $n = O(|T|^{\frac 2 3})$.