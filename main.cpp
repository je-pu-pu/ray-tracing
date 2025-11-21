/*********************************************************************
 * @file   main.cpp
 * @brief  レイトレーシングの練習。右手座標系。
 * @date   2025-10-26
 *********************************************************************/


#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // https://github.com/nothings/stb

// #define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING
#include "Eigen/Dense"

#include <iostream>
#include <fstream>
#include <optional>
#include <vector>
#include <numbers>
#include <chrono>
#include <format>
#include <random>

typedef float Real;
typedef Eigen::Vector3f Vector;
typedef Vector Color;
typedef std::vector< Color > Image;

static constexpr Real operator "" _r( long double x )
{
	return static_cast< Real >( x );
}

constexpr int IMAGE_WIDTH = 1200; /// 出力する画像の横幅
constexpr int IMAGE_HEIGHT = 800; /// 出力する画像の高さ
constexpr Real IMAGE_ASPECT = static_cast< Real >( IMAGE_WIDTH ) / static_cast< Real >( IMAGE_HEIGHT ); /// 画像のアスペクト比
constexpr int IMAGE_SAMPLE_COUNT = 1; /// マルチサンプリングのサンプル数

static constexpr Real degree_to_radian( Real degrre ) { return degrre * std::numbers::pi_v< Real > / 180._r; }
static constexpr Real radian_to_degree( Real radian ) { return radian * 180._r / std::numbers::pi_v< Real >; }

struct Ray
{
	Vector origin;
	Vector direction;

	Vector at( Real t ) const
	{
		return origin + direction * t;
	}
};

struct Object;

struct Hit
{
	const Ray* ray;
	Real distance; /// レイの始点から交差点までの距離
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
	 * レイと球の交差判定
	 * 
	 * @todo 理解する
	 * @param ray 判定するレイ
	 * @return レイと球が交差しない場合は std::nullopt を返す。交差する場合は Hit 構造体を返す。
	 */
	std::optional< Hit > intersect( const Ray& ray ) const override
	{
		const Vector oc = ray.origin - position;
		float a = ray.direction.dot( ray.direction );
		float b = 2._r * ray.direction.dot( oc );
		float c = oc.dot( oc ) - std::powf( radius, 2 );
		float D = b * b - 4 * a * c;
		
		if ( D < 0 )
		{
			return std::nullopt;
		}

		Real t1 = ( -b - sqrtf( D ) ) / ( 2._r * a );
		Real t2 = ( -b + sqrtf( D ) ) / ( 2._r * a );

		Real t = ( t1 > 0 ) ? t1 : ( ( t2 > 0 ) ? t2 : -1 );
		
		if ( t < 0 )
		{
			return std::nullopt;
		}

		return Hit{ .ray = & ray, .distance = t, .object = this };
	}
};

struct Scene
{
	std::vector< Sphere > objects = {
		Sphere( Vector( -3.5_r, 0.0_r, 0 ), 0.25_r, Color( 1.0_r, 0.25_r, 0.25_r ) ),
		Sphere( Vector( -2.5_r, 0.0_r, 0 ), 0.5_r, Color( 1.0_r, 0.25_r, 0.25_r ) ),
		Sphere( Vector(  0.0_r, 0.0_r, 0 ), 1.0_r, Color( 0.25_r, 1.0_r, 0.25_r ) ),
		Sphere( Vector(  2.5_r, 0.0_r, 0 ), 2.0_r, Color( 0.25_r, 0.25_r, 1.0_r ) ),
		Sphere( Vector(  0.0_r, -101.0_r, 0 ), 100._r, Color( 0.75_r, 0.75_r, 0.75_r ) ),
	};

	std::optional< Hit > intersect( const Ray& ray ) const
	{
		Hit hit{ .ray = & ray, .distance = std::numeric_limits< Real >::max(), .object = nullptr };

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

static void save_ppm( const std::string& file_name, Color* image )
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

static void save_image( const Image& image )
{
	std::vector< std::uint8_t > data( IMAGE_WIDTH * IMAGE_HEIGHT * 3 );

	int n = 0;

	for ( const Color& color: image )
	{
		data[ n * 3 + 0 ] = static_cast< std::uint8_t >( color.x() * 255 );
		data[ n * 3 + 1 ] = static_cast< std::uint8_t >( color.y() * 255 );
		data[ n * 3 + 2 ] = static_cast< std::uint8_t >( color.z() * 255 );

		n++;
	}

	const auto now = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() ) }.get_local_time();
	stbi_write_png( std::format( "{:%Y%m%d-%H%M}.png", now ).c_str(), IMAGE_WIDTH, IMAGE_HEIGHT, 3, data.data(), IMAGE_WIDTH * 3 );
}

int main( int, char** )
{
	std::random_device random_device;
	std::mt19937 random_generator{ random_device() };
	std::uniform_real_distribution< Real > random_distribution{ -0.5_r, 0.5_r };

	/*
	Vector a{ 1, 2, 3 };

	a.normalize();

	std::cout << "a dot a =" << a.dot( a ) << std::endl;
	std::cout << "a * a =" << a.norm() * a.norm() << std::endl;

	return 0;
	*/

	std::vector< Color > image{ IMAGE_WIDTH * IMAGE_HEIGHT };

	Scene scene;

	Vector camera_position{ 0, 0, 3 };
	Vector camera_direction = Vector{ 0, 0, -1 }.normalized();
	Vector camera_up = Vector{ 0, 1, 0 }.normalized();
	Vector camera_right = camera_direction.cross( camera_up ).normalized();
	constexpr Real camera_fov = 90._r;

	Vector screen_center{ 0.f, 0.f, 0.f };

	#pragma omp parallel for
	for ( int y = 0; y < IMAGE_HEIGHT; y++ )
	{
		std::cout << "Rendering ( y = " << y << " ) ( " << ( 100.f * y / ( IMAGE_HEIGHT - 1 ) ) << " % )" << std::endl;

		for ( int x = 0; x < IMAGE_WIDTH; x++ )
		{
			Color color{ 0, 0, 0 };

			for ( int n = 0; n < IMAGE_SAMPLE_COUNT; n++ )
			{
				const Real y_offset = IMAGE_SAMPLE_COUNT > 1 ? random_distribution( random_generator ) : 0;
				const Real x_offset = IMAGE_SAMPLE_COUNT > 1 ? random_distribution( random_generator ) : 0;

				// カメラの中心からスクリーン上の y 座標に対応する位置までの回転角度 ( カメラの X 軸回転 )
				const Real yr = ( camera_fov / 2._r ) - camera_fov * ( static_cast< Real >( y + y_offset ) / ( IMAGE_HEIGHT - 1 ) );
				// std::cout << yr << std::endl;

				const Eigen::AngleAxis< Real > rot_y( degree_to_radian( yr ), camera_right );

				// カメラの中心からスクリーン上の x 座標に対応する位置までの回転角度 ( カメラの Y 軸回転 )
				const Real xr = ( camera_fov * IMAGE_ASPECT / 2._r ) - camera_fov * IMAGE_ASPECT * ( static_cast< Real >( x + x_offset ) / ( IMAGE_WIDTH - 1 ) );
				// std::cout << xr << std::endl;

				const Eigen::AngleAxis< Real > rot_x( degree_to_radian( xr ), camera_up );
				// std::cout << d << std::endl;

				Ray ray{ camera_position, ( rot_x * rot_y * camera_direction ).normalized() };

				auto hit = scene.intersect( ray );

				if ( hit )
				{
					Vector N = ( hit.value().ray->at( hit.value().distance ) - hit.value().object->position ).normalized();
					color += 0.5_r * ( N + Vector( 1._r, 1._r, 1._r ) );

					// color += hit.value().object->color;
				}
				else
				{
					// color += Color( static_cast< Real >( x ) / ( IMAGE_WIDTH - 1 ), 0, static_cast< Real >( y ) / ( IMAGE_HEIGHT - 1 ) );
				}
			}

			image[ y * IMAGE_WIDTH + x ] = color / static_cast< Real >( IMAGE_SAMPLE_COUNT );
		}
	}

	save_image( image );

	return 0;
}