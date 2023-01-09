#pragma once

#include <vector>

#include "surface.h"
#include "template.h"

namespace Tmpl8
{	
	/// <summary>
	/// Each Collidable object has a specific type it belongs to.
	/// </summary>
	enum class CollidableType
	{
		PLAYER,
		PLAYER_POINT,
		CAMERA,
		OBSTACLE_VISIBLE,
		OBSTACLE_HIDDEN,
		OBSTACLE_DANGEROUS,
		OBSTACLE_MOVING_VISIBLE,
		OBSTACLE_MOVING_HIDDEN,
		OBSTACLE_UNREACHABLE,
		FINISH_LINE,
		PICKUP,
		GLOW_ORB_FULL,
		GLOW_ORB_TEMP,
		GLOW_ORB_SAFE,
		GLOW_ORB_PICKUP,
		UNKNOWN,
		NONE,
	};

	/// <summary>
	/// Used to identify the collision layer & mask of each Collidable.
	/// </summary>
	enum class CollisionType
	{		
		PLAYER,
		CAMERA,
		BOTH,
		NONE,
	};


	/// <summary>
	/// A data package used in the constructor of the Collidable class.
	/// </summary>
	struct CollidableInfo
	{
		/// <summary>
		/// Constructor with a single collision mask.
		/// </summary>
		/// <param name="collidable_type">Specific type of the Collidable.</param>
		/// <param name="collision_layer">For collision detection, the group or layer does this Collidable belong to.</param>
		/// <param name="m_collision_mask">For collision detection, the group or layer this Collidable interacts with.</param>
		/// <param name="draw_order">The order this will be drawn to the screen. Low numbers are drawn before high numbers.</param>
		/// <param name="m_center">The centerponit of the Collidable.</param>
		CollidableInfo(CollidableType collidable_type, CollisionType collision_layer, CollisionType collision_mask, int draw_order, vec2 m_center = vec2{ 0.0f, 0.0f }) :
			m_collidable_type{ collidable_type },
			m_collision_layer{ collision_layer },
			m_center{ m_center },
			m_draw_order{ draw_order }
		{	
			m_collision_mask.push_back(collision_mask);
		};

		/// <summary>
		/// Constructor with multiple collision masks.
		/// </summary>
		CollidableInfo(CollidableType collidable_type, CollisionType collision_layer, std::vector<CollisionType>& m_collision_mask, int draw_order, vec2 m_center = vec2{ 0.0f, 0.0f }) :
			m_collidable_type{ collidable_type },
			m_collision_layer{ collision_layer },
			m_collision_mask{ m_collision_mask },
			m_center{ m_center },
			m_draw_order{ draw_order }
		{	};

		// See Collidable class for description of each variable here.
		CollidableType m_collidable_type{ CollidableType::NONE };
		CollisionType m_collision_layer{ CollisionType::NONE };
		std::vector<CollisionType> m_collision_mask;
		vec2 m_center{ 0.0f, 0.0f };
		int m_draw_order{ 0 };
	};


	/// <summary>
	/// A base class for all objects that require collision checking.
	/// </summary>
	class Collidable
	{
		/// <summary>
		/// Makes it more legible what is being passed to the Collidable constructor in derived classes.
		/// m_collision_layer and m_collision_mask use the same enums as their type.
		/// </summary>
		using CollisionLayer = CollisionType;
		using CollisionMask = CollisionType;


	public:
		// Auto-generated id. Unique among every Collidable.
		int m_id{ 0 };

		// The specific type of Collidable this is.
		CollidableType m_collidable_type{ CollidableType::UNKNOWN };

		// What kind of collision object this is. This is stored as a bitflag.
		int m_collision_layer{ 0 };

		// What kind of collision object(s) this interacts with. This is stored as a bitflag.
		int m_collision_mask{ 0 };

		// Center of the Collidable.
		vec2 m_center{ 0.0f, 0.0f };

		// How far (in pixels) has this moved since last frame. Mostly used for moving obstacles.
		vec2 m_delta_position{ 0.0f, 0.0f };

		// The order to draw the Collidables in. Drawn in ascending order.
		int m_draw_order{ 0 };

		// The properties of the collision box.
		int left{ 0 }, right{ 0 }, top{ 0 }, bottom{ 0 };

		// Stores direction of each adjacent neighbor in a bitflag.
		int m_neighbors{ 0 };

		// If the Collidable is currently active. Mostly used by glow orbs.
		bool m_is_active{ true };


		/// <summary>
		/// Constructor. Utilizes the struct CollidableInfo to make initialization easier through multiple derived classes such as the glow orbs and moving obstacles.
		/// </summary>
		Collidable(CollidableInfo collidable_info);


		/// <summary>
		/// Constructor. Utilizes the struct CollidableInfo to make initialization easier through multiple derived classes such as the glow orbs and moving obstacles.
		/// Takes additoinal arguments for setting up the left/right/top/bottom collision box properties.
		/// </summary>
		Collidable(CollidableInfo collidable_info, int x, int y, int tile_size);


		/// <summary>
		/// Simple draw function for all Collidables. Objects using this will generally draw to a Surface once during level initialization
		/// and will not change their position or physical properties.
		/// </summary>
		virtual void Draw(Surface* screen) {};


		/// <summary>
		/// Complex draw function for all Collidables. Object using this have dynamic physical properties or positions.
		/// The position of the camera, as well as its bounding box, are passed in with an opacity value.
		/// </summary>
		virtual void Draw(Surface* visible_layer, int camera_left_bound, int camera_top_bound, int inbound_left, int inbound_top, int inbound_right, int inbound_bottom, float opacity = 1.0f) {};


		/// <summary>
		/// Called when the collision system detects this object has collided with another. Both objects are passed a reference to each other.
		/// It is up to the objects on what they will do with or in reponse to their reference.
		/// </summary>
		virtual void RegisterCollision(Collidable*& collision) {};


		/// <summary>
		/// Called after the collision system has registered all collisions.
		/// Intended time for all objects to react to any collisions they have registered.
		/// </summary>
		virtual void ResolveCollisions() {};
				
		
	private:
		// Provides the m_id value for every Collidable.
		static int id_generator;


		/// <summary>
		/// Called by constructor to get bitflag value based on CollisionMask(s).
		/// </summary>
		int GetCollisionId(std::vector<CollisionType>& collidables);


		/// <summary>
		/// Called by constructor to get bitflag value based on CollisionLayer or single CollisionMask.
		/// </summary>
		int GetCollisionId(CollisionType& collidable);
	};

};

