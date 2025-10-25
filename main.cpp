/*********************************************************************
 * @file   main.cpp
 * @brief  レイトレーシングの練習。右手座標系。
 * @date   2025-10-26
 *********************************************************************/

#define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING

#include "Eigen/Dense"
#include <iostream>
#include <fstream>
#include <optional>
#include <vector>
#include <numbers>

typedef float Real;
typedef Eigen::Vector3f Vector;
typedef Vector Color;

constexpr Real operator "" _r( long double x )
{
	return static_cast< Real >( x );
}

constexpr int IMAGE_WIDTH = 1200; /// 出力する画像の横幅
constexpr int IMAGE_HEIGHT = 800; /// 出力する画像の高さ
constexpr Real IMAGE_ASPECT = static_cast< Real >( IMAGE_WIDTH ) / static_cast< Real >( IMAGE_HEIGHT ); /// 画像のアスペクト比

constexpr Real degree_to_radian( Real degrre ) { return degrre * std::numbers::pi_v< Real > / 180._r; }
constexpr Real radian_to_degree( Real radian ) { return radian * 180._r / std::numbers::pi_v< Real >; }

struct Ray
{
	Vector origin;
	Vector direction;
};

struct Object;

struct Hit
{
	Real distance;
	const Object* object;
};

struct Object
{
	Vector position;
	Color color;

	Object( const Vector& p, const Color& c )
		: position( p )
		, color( c )
	{

	}

	virtual ~Object()
	{

	}

	virtual std::optional< Hit > intersect( const Ray& ) const = 0;
};

struct Sphere : public Object
{
	Real radius;

	Sphere( const Vector& p, Real r, const Color& c )
		: Object( p, c )
		, radius( r )
	{

	}

	~Sphere() override
	{

	}

	/**
	 * .
	 * 
	 * @todo 実装する
	 * @param ray
	 * @return 
	 */
	std::optional< Hit > intersect( const Ray& ray ) const override
	{
		return std::nullopt;

		Hit hit;
		hit.object = this;

		return std::nullopt;
	}
};

struct Scene
{
	std::vector< Sphere > objects = {
		Sphere( Vector( 0, 0, 0 ), 1, Color( 1, 0, 0 ) )
	};

	std::optional< Hit > intersect( const Ray& ray ) const
	{
		Hit hit;
		hit.distance = std::numeric_limits< Real >::max();
		hit.object = nullptr;

		for ( const auto& object : objects )
		{
			auto result = object.intersect( ray );

			if ( result && result->distance < hit.distance )
			{
				hit = result.value();
			}
		}

		if ( hit.object )
		{
			return hit;
		}

		return std::nullopt;
	}
};

void save_ppm( const std::string& file_name, Color* image )
{
	std::ofstream ofs( file_name );

	ofs << "P3\n" << IMAGE_WIDTH << " " << IMAGE_HEIGHT << "\n255\n";

	for ( int n = 0; n < IMAGE_WIDTH * IMAGE_HEIGHT; n++ )
	{
		ofs << static_cast< int >( image[ n ].x() * 255 ) << " ";
		ofs << static_cast< int >( image[ n ].y() * 255 ) << " ";
		ofs << static_cast< int >( image[ n ].z() * 255 ) << "\n";
	}
}

int main( int, char** )
{
	/*
	Vector a{ 1, 2, 3 };

	a.normalize();

	std::cout << "a dot a =" << a.dot( a ) << std::endl;
	std::cout << "a * a =" << a.norm() * a.norm() << std::endl;

	return 0;
	*/

	std::vector< Color > image{ IMAGE_WIDTH * IMAGE_HEIGHT };

	Scene scene;

	Vector camera_position{ 0, 0, 5 };
	Vector camera_direction = Vector{ 0, 0, -1 }.normalized();
	Vector camera_up = Vector{ 0, 1, 0 }.normalized();
	Vector camera_right = camera_direction.cross( camera_up ).normalized();
	constexpr Real camera_fov = 90._r;

	Vector screen_center{ 0.f, 0.f, 0.f };

	#pragma omp parallel for
	for ( int y = 0; y < IMAGE_HEIGHT; y++ )
	{
		std::cout << "Rendering ( y = " << y << " ) ( " << ( 100.f * y / ( IMAGE_HEIGHT - 1 ) ) << " % )" << std::endl;

		// カメラの中心からスクリーン上の y 座標に対応する位置までの回転角度 ( カメラの X 軸回転 )
		const Real yr = ( camera_fov / 2._r ) - camera_fov * ( static_cast< Real >( y ) / ( IMAGE_HEIGHT - 1 ) );
		// std::cout << yr << std::endl;

		const Eigen::AngleAxis< Real > rot_y( degree_to_radian( yr ), camera_right );

		for ( int x = 0; x < IMAGE_WIDTH; x++ )
		{
			// カメラの中心からスクリーン上の x 座標に対応する位置までの回転角度 ( カメラの Y 軸回転 )
			const Real xr = ( camera_fov * IMAGE_ASPECT / 2._r ) - camera_fov * IMAGE_ASPECT * ( static_cast< Real >( x ) / ( IMAGE_WIDTH - 1 ) );
			// std::cout << xr << std::endl;

			const Eigen::AngleAxis< Real > rot_x( degree_to_radian( xr ), camera_up );
			// std::cout << d << std::endl;

			Ray ray{ camera_position, rot_x * rot_y * camera_direction };

			auto hit = scene.intersect( ray );

			if ( hit )
			{
				image[ y * IMAGE_WIDTH + x ] = hit.value().object->color;
			}
			else
			{
				image[ y * IMAGE_WIDTH + x ] = Color( static_cast< Real >( x ) / ( IMAGE_WIDTH - 1 ), 0, static_cast< Real >( y ) / ( IMAGE_HEIGHT - 1 ) );
			}

			// ofs << "255 127 127\n";
			// ofs << "255 0 255\n";
		}
	}

	save_ppm( "result.ppm", & image[ 0 ] );

	return 0;
}