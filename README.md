# レイトレーシング

[レイをトレースしたい　第三回　球の描画 - Qiita](https://qiita.com/mosamosa/items/ffd89fb8a424dd83c415)
[レイトレーシングの基本を言語化して理解したい。 - Qiita](https://qiita.com/mosamosa/items/8b49903e40638674ed1f)
[Chapter4. 最初の実装 | The Textbook of RayTracing @TDU](https://knzw.tech/raytracing/?page_id=1243#toc-03476ddd7e79e9c8fa7d759966a93ba4-5)

[球面の方程式に関する5つの公式と具体例 | 高校数学の美しい物語](https://manabitimes.jp/math/1029)

[GitHub - githole/edupt: パストレーシングベースの物理ベースレンダラ。](https://github.com/githole/edupt)
[https://kagamin.net/hole/edupt/edupt_v103.pdf](https://kagamin.net/hole/edupt/edupt_v103.pdf)

[はじめに - 週末レイトレーシング 第一週 (翻訳) - inzkyk.xyz](https://inzkyk.xyz/ray_tracing_in_one_weekend/week_1/1_1_overview/)

[レイトレーシング入門](https://zenn.dev/mebiusbox/books/8d9c42883df9f6)

---

# 球の方程式

球の中心が原点 ( 0, 0, 0 ) の球は以下の式で表すことができる。

$$
x^2 + y^2 + z^2 = r^2
$$

* $( x, y, z )$ : 球の表面上の座標
* $r$ : 球の半径

---

球の中心の座標を ( cx, cy, cz ) とすると、以下の式で表すことができる。

$$
( x - cx )^2 + ( y - cy )^2 + ( z - cz )^2 = r^2
$$

* $( cx, cy, cz )$ : 球の中心の座標  
* $r > 0$ : 球の半径

---

ベクトルを使うと、以下の式で表すことができる。

$$
( \vec{p} - \vec{c} ) \cdot ( \vec{p} - \vec{c} ) = r^2
\\
\|\vec{p} - \vec{c}\|^2 = r^2
\\
\|\vec{p} - \vec{c}\| = r

$$

* $\vec{p}$ : 球の表面上の座標
* $\vec{c}$ : 球の中心の座標

---

# レイの方程式

$$
P(t) = \vec{o} + t\vec{d}
$$

* $P(t)$ : パラメータ $t$ におけるレイ上の点
* $t$ : レイの始点からの距離を表すパラメータ
* $\vec{o}$ : レイの始点
* $\vec{d}$ : レイの方向

---

# 球の方程式にレイの方程式を代入する

レイを始点 $\vec{o}$ から、方向 $\vec{d}$ へ、距離 $t$ だけ進め、  
そのレイ上の点 $P(t)$ が、球の表面上の座標 $\vec{p}$ と一致する場合、  
レイと球は交差していると言える。

$$
\| \vec{p} - \vec{c} \| ^2 = r^2 \\
= \| P(t) - \vec{c} \| ^2 = r^2 \\
= \| \vec{o} + t \vec{d} - \vec{c} \| ^2 = r^2
$$

ここで、簡単のために

$$
\vec{oc} = \vec{o} - \vec{c}
$$

とおくと、以下のように書ける。

$$
\| \vec{oc} + t \vec{d} \| ^2 = r^2
$$

これは以下のように展開できる。

$$
( \vec{oc} + t \vec{d} ) \cdot ( \vec{oc} + t \vec{d} ) = r^2
$$

分配法則により、

$$
\vec{oc} \cdot \vec{oc} \quad + \quad \vec{oc} \cdot ( t \vec{d} ) \quad + \quad ( t \vec{d} ) \cdot \vec{oc} \quad + \quad ( t \vec{d} ) \cdot ( t \vec{d} ) \quad = \quad r^2
$$

スカラー $t$ をくくり出し、

$$
\vec{oc} \cdot \vec{oc} \quad + \quad 2 t ( \vec{oc} \cdot \vec{d} ) \quad + \quad t^2( \vec{d} \cdot \vec{d} ) \quad = \quad r^2
$$

t についての二次方程式の形に整理し、

$$
t^2( \vec{d} \cdot \vec{d} ) \quad + \quad 2 t ( \vec{oc} \cdot \vec{d} ) \quad + \quad \vec{oc} \cdot \vec{oc} - r^2 \quad = \quad0
$$

> 「t についての二次方程式」というのは、未知数（解きたい変数）が t であり、その t が 2 乗で登場する方程式