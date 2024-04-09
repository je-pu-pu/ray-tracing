#define _SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING

#include "Eigen/Core"
#include <iostream>
#include <fstream>
#include <optional>
#include <vector>

const int w = 1200;
const int h = 800;

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
	Object* object;
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

	std::optional< Hit > intersect( const Ray& ray ) const override
	{
		// ray.

		return std::nullopt;
	}
};

struct Scene
{
	std::vector< std::unique_ptr< Object > > objects{
		std::make_unique< Sphere >( Vector( 0, 0, 0 ), 1, Color( 1, 0, 0 ) )
	};

	std::optional< Hit > intersect( const Ray& ray ) const
	{
		Hit hit;
		hit.distance =  std::numeric_limits< Real >::max();
		hit.object = nullptr;

		for ( const auto& object : objects )
		{
			auto result = object->intersect( ray );

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

	ofs << "P3\n" << w << " " << h << "\n255\n";

	for ( int n = 0; n < w * h; n++ )
	{
		ofs << static_cast< int >( image[ n ].x() * 255 ) << " ";
		ofs << static_cast< int >( image[ n ].y() * 255 ) << " ";
		ofs << static_cast< int >( image[ n ].z() * 255 ) << "\n";
	}
}

int main( int, char** )
{
	std::vector< Color > image{ w * h };

	for ( int y = 0; y < h; y++ )
	{
		for ( int x = 0; x < w; x++ )
		{
			image[ y * w + x ] = Color( static_cast< Real >( x ) / ( w - 1 ), 0, static_cast< Real >( y ) / ( h - 1 ) );

			// ofs << "255 127 127\n";
			// ofs << "255 0 255\n";
		}
	}

	save_ppm( "result.ppm", & image[ 0 ] );
}