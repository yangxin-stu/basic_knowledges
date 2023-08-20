#### LSTM 公式
$$
\begin{equation}
\begin{array}{ll}
\hat{C}_t = tanh(W_C\otimes[h_{t-1},x_t]+b_C) \\[2ex] % 输入信息： 记忆门--从融合信息i_t中提取信息，作用于输入信息
i_t=\sigma(W_i\otimes[h_{t-1},x_t]+b_i) \\[2ex] % 记忆门，输入信息融合--融合h_{t-1},x_t的信息，或称融合门
f_t=\sigma(W_f\otimes[h_{t-1},x_t]+b_f) \\[2ex] % 遗忘门--从cell中忘记东西，记忆与以遗忘都是针对cell的，作用于cell信息
C_t=f_t * C_{t-1}+i_t * \hat{C}_t \\[2ex] % cell 状态更新，包含两部分：从上一个cell忘记一些信息，从当前融合信息提取需要记忆的信息
o_t=\sigma(W_o\otimes[h_{t-1},x_t]+b_o) \\[2ex] % 输出门--负责从更新后的cell提取输出信息
h_t=o_t * tanh(C_t) % 隐状态输出--利用当前的cell状态获取输出。所以cell式操作的核心。记忆，遗忘，输出对cell服务的
\end{array}
\end{equation}
$$


#### convLSTM公式
$$
\begin{equation}
\begin{array}{ll}
\hat{C}_t = tanh(W_C\odot[h_{t-1},x_t]+b_C) \\[2ex] % 输入信息： 记忆门--从融合信息i_t中提取信息，作用于输入信息
i_t=\sigma(W_i\odot[h_{t-1},x_t]+b_i) \\[2ex] % 记忆门，输入信息融合--融合h_{t-1},x_t的信息，或称融合门
f_t=\sigma(W_f\odot[h_{t-1},x_t]+b_f) \\[2ex] % 遗忘门--从cell中忘记东西，记忆与以遗忘都是针对cell的，作用于cell信息
C_t=f_t * C_{t-1}+i_t * \hat{C}_t \\[2ex] % cell 状态更新，包含两部分：从上一个cell忘记一些信息，从当前融合信息提取需要记忆的信息
o_t=\sigma(W_o\odot[h_{t-1},x_t]+b_o) \\[2ex] % 输出门--负责从更新后的cell提取输出信息
h_t=o_t * tanh(C_t) % 隐状态输出--利用当前的cell状态获取输出。所以cell式操作的核心。记忆，遗忘，输出对cell服务的
\end{array}
\end{equation}
$$


#### 机器学习中常用公式 #常用公式
$$h(\theta)=\sum_{j=0}^n \theta_jx_j$$

$$J(\theta)=\frac1{2m}\sum_{i=0}(y^i-h_\theta(x^i))^2$$


$$
\frac{\partial J(\theta)}{\partial\theta_j}=
-\frac1m\sum_{i=0}^m(y^i-h_\theta(x^i))x^i_j 
$$


$$
f(n) =
    \begin{cases}
    n/2,  & \text{if $n$ is even} \\
    3n+1, & \text{if $n$ is odd}
    \end{cases}
$$

$$f(x)=\begin{cases} x = \cos(t) \\y = \sin(t) \\ z = \frac xy \end{cases}$$


$$
\left\{ 
    \begin{array}{c}
        a_1x+b_1y+c_1z=d_1 \\ 
        a_2x+b_2y+c_2z=d_2 \\ 
        a_3x+b_3y+c_3z=d_3
    \end{array}
\right. 
$$


$$X=\left(
        \begin{matrix}
            x_{11} & x_{12} & \cdots & x_{1d}\\
            x_{21} & x_{22} & \cdots & x_{2d}\\
            \vdots & \vdots & \ddots & \vdots\\
            x_{m1} & x_{m2} & \cdots & x_{md}\\
        \end{matrix}
    \right)
    =\left(
         \begin{matrix}
                x_1^T \\
                x_2^T \\
                \vdots\\
                x_m^T \\
            \end{matrix}
    \right)
$$


$$
\begin{align}
\frac{\partial J(\theta)}{\partial\theta_j}
& = -\frac1m\sum_{i=0}^m(y^i-h_\theta(x^i)) \frac{\partial}{\partial\theta_j}(y^i-h_\theta(x^i)) \\
& = -\frac1m\sum_{i=0}^m(y^i-h_\theta(x^i)) \frac{\partial}{\partial\theta_j}(\sum_{j=0}^n\theta_jx_j^i-y^i) \\
& = -\frac1m\sum_{i=0}^m(y^i-h_\theta(x^i))x^i_j
\end{align}
$$


#### 代数
$$ \left(x-2\right)\left(x+4\right) $$

$$\sqrt{a^2+b^2}$$

$$\left ( \frac{a}{b}\right )^{n}= \frac{a^{n}}{b^{n}}$$

$$\frac{a}{b}\pm \frac{c}{d}= \frac{ad \pm bc}{bd}$$

$$\frac{x^{2}}{a^{2}}-\frac{y^{2}}{b^{2}}=1$$

$$x ={-b \pm \sqrt{b^2-4ac}\over 2a}$$

$$y-y_{1}=k \left( x-x_{1}\right)$$

#### 数列
$$a_{n}=a_{1}q^{n-1}$$

$$a_{n}=a_{1}+ \left( n-1 \right) d$$

$$S_{n}=na_{1}+\frac{n \left( n-1 \right)}{{2}}d$$

$$ S_{n}=\frac{n \left( a_{1}+a_{n}\right)}{2}$$

$$\frac{1}{n \left( n+k \right)}= \frac{1}{k}\left( \frac{1}{n}-\frac{1}{n+k}\right)$$

#### 三角函数
$$\sin \alpha + \sin \beta =2 \sin \frac{\alpha + \beta}{2}\cos \frac{\alpha - \beta}{2}$$

$$a^{2}=b^{2}+c^{2}-2bc\cos A$$

$$\text{sin}^{2}\frac{\alpha}{2}=\frac{1- \text{cos}\alpha}{2}$$

$$\frac{\sin A}{a}=\frac{\sin B}{b}=\frac{\sin C}{c}=\frac{1}{2R}$$

#### 积分
$$\frac{\mathrm{d}}{\mathrm{d}x}x^n=nx^{n-1}$$

$$\frac{\mathrm{d}}{\mathrm{d}x}\ln(x)=\frac{1}{x}$$

$$\int k\mathrm{d}x = kx+C$$

$$\int \frac{1}{1+x^{2}}\mathrm{d}x= \arctan x +C$$

$$f(x) = \int_{-\infty}^\infty \hat f(x)\xi\,e^{2 \pi i \xi x} \,\mathrm{d}\xi$$

$$\int \frac{1}{\sqrt{1-x^{2}}}\mathrm{d}x= \arcsin x +C$$

#### 不等式
$$a > b,b > c \Rightarrow a > c$$

$$a > b > 0,c > d > 0 \Rightarrow ac bd$$

$$a \gt b \gt 0,n \in N^{\ast},n \gt 1$$

$$ a,b \in R^{+}\Rightarrow \frac{a+b}{{2}}\ge \sqrt{ab}$$

#### 矩阵 #矩阵

$$ \begin{pmatrix} 1 & 0 \\ 0 & 1 \end{pmatrix}$$

$$A_{m\times n}= \begin{bmatrix} a_{11}& a_{12}& \cdots & a_{1n} \\ a_{21}& a_{22}& \cdots & a_{2n} \\ \vdots & \vdots & \ddots & \vdots \\ a_{m1}& a_{m2}& \cdots & a_{mn} \end{bmatrix} =\left [ a_{ij}\right ] 
$$

$$
        \begin{matrix}
        1 & x & x^2 \\
        1 & y & y^2 \\
        1 & z & z^2 \\
        \end{matrix}
$$

