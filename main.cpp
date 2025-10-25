#define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING

#include "Eigen/Core"
#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

const int IMAGE_WIDTH = 1200; /// 出力する画像の横幅
const int IMAGE_HEIGHT = 800; /// 出力する画像の高さ

typedef float Real;
typedef Eigen::Vector3f Vector;
typedef Vector Color;

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

Real operator "" _r( long double x )
{
	return static_cast< Real >( x );
}

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

	Vector camera_position{ 0.f, 0.f, -5.f };
	Vector screen_center{ 0.f, 0.f, 0.f };

	#pragma omp parallel for
	for ( int y = 0; y < IMAGE_HEIGHT; y++ )
	{
		std::cout << "Rendering ( y = " << y << " ) ( " << ( 100.f * y / ( IMAGE_HEIGHT - 1 ) ) << " % )" << std::endl;

		for ( int x = 0; x < IMAGE_WIDTH; x++ )
		{
			Vector screen_position{ 0, 0, 0 }; // x, y に対応するスクリーンの位置

			Ray ray{ camera_position, ( screen_position - camera_position ).normalized() };

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
}