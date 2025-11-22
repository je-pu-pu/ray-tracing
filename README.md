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

球の中心が原点 $( 0, 0, 0 )$ にある場合、球面上の点 $( x, y, z )$ は次の方程式を満たす。

$$
x^2 + y^2 + z^2 = r^2
$$

* $( x, y, z )$ : 球面上の点の座標
* $r$ : 球の半径

---

球の中心を $( cx, cy, cz )$ とすると、以下の式で表すことができる。

$$
( x - cx )^2 + ( y - cy )^2 + ( z - cz )^2 = r^2
$$

* $( cx, cy, cz )$ : 球の中心の座標  
* $r > 0$ : 球の半径

---

ベクトルを使うと、よりコンパクトに、以下の式で表すことができる。

$$
( \vec{p} - \vec{c} ) \cdot ( \vec{p} - \vec{c} ) = r^2
$$

* $\vec{p}$ : 球の表面上の座標
* $\vec{c}$ : 球の中心の座標


また、上記の式は、ベクトルの内積の性質より、ベクトルの大きさ（ノルム）使って次のように書き換えられる。

$$
\|\vec{p} - \vec{c}\|^2 = r^2
$$

これは、球面上の点 $\vec{p}$ の球の中心 $\vec{c}$ からの距離の 2 乗が、球の半径の 2 乗に等しいことを表している。  
ベクトルの大きさ（ノルム）は常に非負なので、両辺の平方根を取ることで

$$
\|\vec{p} - \vec{c}\| = r
$$

が得られる。  
これは「中心からの距離がちょうど $r$ の点全体」が球面であることを意味している。

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

レイを始点 $\vec{o}$ から、方向 $\vec{d}$ へ、距離 $t$ だけ進めた点 $P(t)$ が球面上にある場合、  
レイと球は交差していることになる。

このとき、球の方程式の $\vec{p}$ を $P(t)$ に置き換えると、

$$
\| \vec{p} - \vec{c} \|^2 = r^2 \\
$$

$$
\Rightarrow \| P(t) - \vec{c} \|^2 = r^2 \\
$$

$$
\Rightarrow \| \vec{o} + t \vec{d} - \vec{c} \|^2 = r^2
$$

ここで、計算を分かりやすくするために、

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

> t についての二次方程式  
> というのは、未知数（解きたい変数）が t であり、その t が 2 乗で登場する方程式

これを一般的な二次方程式の形にすると、

$$
at^2 \quad + \quad bt \quad + \quad c \quad = \quad 0
$$

で、各定数は以下のように定義される。

$$
\begin{cases}
    a = \vec{d} \cdot \vec{d} \\
    b = 2 ( \vec{oc} \cdot \vec{d} ) \\
    c = \vec{oc} \cdot \vec{oc} - r^2
\end{cases}
$$

# 交差の有無の判定

二次方程式の解の公式は以下の通り。

$$
t = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}
$$

ここで、判定式

$$
\Delta = b^2 - 4ac
$$

を考える。

* $\Delta < 0$ : 実数解なし : レイは球と交差していない
* $\Delta = 0$ : 1 つの解がある : レイは球と接している
* $\Delta > 0$ : 2 つの解がある : レイは球を貫通している（交点が２つある）

# 交点の選択

2 つの解 $t_1,t_2$ が出た場合、レイの始点から最も近い交点を採用する。

$$
t = \min(t_1, t_2), \quad \text{ただし } t_1 > 0, \ t_2 > 0
$$

# 交点座標の計算

交点のパラメータ $t$ が求まったら、レイの方程式に代入して座標を求める。

$$
P(t) = \vec{o} + t\vec{d}
$$

# C++ ソースコード例

main.cpp

```c++
/**
 * レイと球の交差判定
 * 
 * @param ray 判定するレイ
 * @return レイと球が交差しない場合は std::nullopt を返す。交差する場合は Hit 構造体を返す。
 */
std::optional< Hit > intersect( const Ray& ray ) const override
{
	const auto oc = ray.origin - position;				// 計算の便宜上、球の中心からレイの始点へのベクトルを求める
	const auto a = ray.direction.dot( ray.direction );	// レイの方向ベクトルの大きさの2乗
	const auto b = 2 * ray.direction.dot( oc );			// レイの方向ベクトルと oc ベクトルの内積に 2 をかけたもの
	const auto c = oc.dot( oc ) - radius * radius;		// oc ベクトルの大きさの 2 乗から球の半径の 2 乗を引いたもの
	const auto D = b * b - 4 * a * c;					// 交差判定の判別式
	
	// D が負の場合、レイと球は交差しない
	if ( D < 0 )
	{
		return std::nullopt;
	}

	// 交差する場合、2 つの交点 t1, t2 を求める
	auto t1 = ( -b - std::sqrt( D ) ) / ( 2 * a );
	auto t2 = ( -b + std::sqrt( D ) ) / ( 2 * a );

	// 交点のうち、レイの始点から最も近い交点を採用する
	auto t = -1._r;

	if ( t1 > 0 && t2 > 0 )
	{
		t = std::min( t1, t2 );
	}
	else if ( t1 > 0 )
	{
		t = t1;
	}
	else if ( t2 > 0 )
	{
		t = t2;
	}
	
	// 交点がレイの始点に近すぎる場合、交差しないものとみなす
	if ( t < 0.001_r )
	{
		return std::nullopt;
	}

	const auto hit_position = ray.at( t );

	return Hit{ .distance = t, .position = hit_position , .normal = ( hit_position - position ).normalized(), .object = this };
}
```