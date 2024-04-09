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

	Object( const Vector& p )
		: position( p )
	{

	}

	virtual std::optional< Hit > intersect( const Ray& ) const = 0;
};

struct Sphere : public Object
{
	Real radius;

	Sphere( const Vector& p, Real r )
		: Object( p )
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
		std::make_unique< Sphere >( Vector( 0, 0, 0 ), 1 )
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

int main( int, char** )
{
	Vector v( 1.2_r, 2.3_r, 3.4_r );
	v.normalize();

	std::cout << "Hello." << std::endl;
	std::cout << v << std::endl;

	std::ofstream ofs( "result.ppm" );

	ofs << "P3\n" << w << " " << h << "\n255\n";

	for ( int y = 0; y < h; y++ )
	{
		for ( int x = 0; x < w; x++ )
		{
			// ofs << "255 127 127\n";
			ofs << "255 0 255\n";
		}
	}
}