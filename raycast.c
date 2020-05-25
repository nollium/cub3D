/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smaccary <smaccary@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 21:24:35 by smaccary          #+#    #+#             */
/*   Updated: 2020/05/25 21:43:22 by smaccary         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

static void	init_raycast(t_vars *vars, int x, t_ray *ray)
{
	long double	camera_x;

	camera_x = 2 * x / (long double)WINDOW_WIDTH - 1.0;
	ray->dir_x = vars->cam.dir_x + vars->cam.plane.x * camera_x;
	ray->dir_y = vars->cam.dir_y + vars->cam.plane.y * camera_x;
	vars->map.x = (int)(vars->cam.x);
	vars->map.y = (int)(vars->cam.y);
	ray->delta_dist_x = fabsl(1 / ray->dir_x);
	ray->delta_dist_y = fabsl(1 / ray->dir_y);
	ray->hit = 0;
}

static void	get_step(t_vars *vars, t_ray *ray)
{
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->side_dist_x = (vars->cam.x - vars->map.x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->side_dist_x =
			(vars->map.x + 1.0 - vars->cam.x) * ray->delta_dist_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->side_dist_y = (vars->cam.y - vars->map.y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->side_dist_y =
			(vars->map.y + 1.0 - vars->cam.y) * ray->delta_dist_y;
	}
}

static void	perform_dda(t_vars *vars, t_ray *ray)
{
	while (ray->hit == 0)
	{
		if (ray->side_dist_x < ray->side_dist_y)
		{
			ray->side_dist_x += ray->delta_dist_x;
			vars->map.x += ray->step_x;
			ray->side = 0;
		}
		else
		{
			ray->side_dist_y += ray->delta_dist_y;
			vars->map.y += ray->step_y;
			ray->side = 1;
		}
		if (vars->map.worldMap[vars->map.x][vars->map.y] - '0' > 0)
			ray->hit = 1;
	}
}
//w -0.15 e -0.05  side = 1|side = 0 n -0.91 s s-0.58
//0.72 1.03 /  0.73 1.10s
static void	get_wall_dist(t_vars *vars, t_ray *ray)
{
	if (ray->side == 0)
		ray->perp_wall_dist =
			(vars->map.x - vars->cam.x + (1.0 - ray->step_x) / 2.0)
				/ ray->dir_x;
	else
		ray->perp_wall_dist =
			(vars->map.y - vars->cam.y + (1.0 - ray->step_y) / 2.0)
				/ ray->dir_y;
}

static void	get_wall_side(t_vars *vars, t_ray *ray)
{
	double angle = atan2(3.0 - (double)vars->cam.y, 4.0 - (double)vars->cam.x) * 180.0 / PI;
	int		side = ray->side;
	double diff;
	double max;
	int corner;
//	diff = fabs(angle - 175.0); //west
//	diff = fabs(angle + 110); //east
//	printf("angle : %lf && side = %d\n", angle, ray->side);
	
	diff = fabs(angle - 45.0);
	max = diff;
	corner = 45;
	diff = fabs(angle - 145.0);
	if (diff < max)
	{
		max = diff;
		corner = 145;
	}
	diff = fabs(angle + 55.0);
	if (diff < max)
	{
		max = diff;
		corner = 55;
	}
	diff = fabs(angle + 135.0);
	if (diff < max)
	{
		max = diff;
		corner = 135;
	}
/*
	if (corner == 45)
	{
		printf((side) ? "West a\n" : "South\n"); // 45
		vars->w_color = (side) ? 0x000000FF : 0x00FF0000;
	}
	else if (corner == 145)
	{
		printf((side) ? "East a\n" : "South\n"); // 145
		vars->w_color = (side) ? 0x00FF00FF : 0x00FF0000;
	}
	else if (corner == 55)
	{
		printf((side) ? "West b\n" : "North a\n"); // 55
		vars->w_color = (side) ? 0x000000FF : 0x0000FF00;
	}
	else if (corner == 135)
	{
		printf((side) ? "East b\n" : "North b\n"); // 135
		vars->w_color = (side) ? 0x00FF00FF : 0x0000FF00;
	}*/

	if (corner == 45)
	{
		printf((side) ? "West a\n" : " North (South a)\n"); // 45
		vars->w_color = (side) ? WEST : NORTH;
	}
	else if (corner == 145)
	{
		printf((side) ? "West (east a)\n" : "South b\n"); // 145
		vars->w_color = (side) ? WEST : SOUTH;
	}
	else if (corner == 55)
	{
		printf((side) ? "East (West b)\n" : "North a\n"); // 55
		vars->w_color = (side) ? EAST : NORTH;
	}
	else if (corner == 135)
	{
		printf((side) ? "East b\n" : "South (North b)\n"); // 135
		vars->w_color = (side) ? EAST : SOUTH;
	}
}

static void	get_texture_coords(t_vars *vars, t_ray *ray)
{
	double wallX; //where exactly the wall was hit
	int texNum;

	//texturing calculations
	texNum = vars->map.worldMap[vars->map.x][vars->map.x] - 1; //1 subtracted from it so that texture 0 can be used!

	//calculate value of wallX
	if (ray->side == 0)
		wallX = vars->cam.y + ray->perp_wall_dist * ray->dir_y;
	else
		wallX = vars->cam.x + ray->perp_wall_dist * ray->dir_x;
	(void)0;
	wallX -= floor((wallX));

	//x coordinate on the texture
	vars->text.x = (int)(wallX * (double)(vars->text.width));
	if(ray->side == 0 && ray->dir_x > 0)
		vars->text.x = vars->text.width - vars->text.x - 1;
	if(ray->side == 1 && ray->dir_y < 0)
		vars->text.x = vars->text.width - vars->text.x - 1;
}

t_ray		raycast(t_ray *ray, t_vars *vars, int x)
{
	init_raycast(vars, x, ray);
	get_step(vars, ray);
	perform_dda(vars, ray);
	get_wall_dist(vars, ray);
	get_texture_coords(vars, ray);
	if (vars->map.x == 4 && vars->map.y == 3)
		get_wall_side(vars, ray);
	else
		vars->w_color = 0;
}
