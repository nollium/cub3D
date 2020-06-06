/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smaccary <smaccary@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/04/17 21:24:35 by smaccary          #+#    #+#             */
/*   Updated: 2020/06/06 22:10:18 by smaccary         ###   ########.fr       */
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

static int	get_wall_side(t_vars *vars, t_ray *ray)
{
	double		angle;
	double		diff;
	double		max;
	int			corner;
	static int	corner_angle[] = {45, 145, -55, -135};
	int			i;

//	printf("angle : %lf && ray->side = %d\n", angle, ray->ray->side);
	i = -1;
	angle = atan2(vars->map.y - (double)vars->cam.y, (double)vars->map.x
		- (double)vars->cam.x) * 180.0 / PI;
	max = DBL_MAX;
	while (++i <= 3)
		if (((diff = fabs(angle - (double)corner_angle[i])) < max))
		{
			max = diff;
			corner = corner_angle[i];
		}
	if (corner == 45)
		return ((ray->side) ? WEST : NORTH);
	else if (corner == 145)
		return ((ray->side) ? WEST : SOUTH);
	else if (corner == -55)
		return ((ray->side) ? EAST : NORTH);
	else if (corner == -135)
		return ((ray->side) ? EAST : SOUTH);
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
	vars->text[ray->w_num].x = (int)
		(wallX * (double)(vars->text[ray->w_num].width));
	if (ray->side == 0 && ray->dir_x > 0)
		vars->text[ray->w_num].x = vars->text[ray->w_num].width - vars->text[ray->w_num].x - 1;
	if (ray->side == 1 && ray->dir_y < 0)
		vars->text[ray->w_num].x = vars->text[ray->w_num].width - vars->text[ray->w_num].x - 1;
}

void		raycast_walls(t_ray *ray, t_vars *vars, int x)
{
	init_raycast(vars, x, ray);
	get_step(vars, ray);
	perform_dda(vars, ray);
	get_wall_dist(vars, ray);
	ray->w_num = get_wall_side(vars, ray);
	get_texture_coords(vars, ray);
	vars->z_buffer[x] = ray->perp_wall_dist;
}

/*
static void swap_sprites(t_sprite *sprite_1, t_sprite *sprite_2)
{
	t_sprite	tmp;
	
	tmp = *sprite_1;
	*sprite_1 = *sprite_2;
	*sprite_2 = tmp;
}*/

/*
** Insertion sort for sorting sprites from further away to player, to closer from player
*/

void 		sort_sprites(int n, t_sprites_sorter *arr) 
{ 
	register int	i;
	register int	j; 
	double 			key;

	i = 0;
	while (++i < n) 
	{ 
		key = arr[i].sprite_distance; 
		j = i - 1; 
		while (j >= 0 && arr[j].sprite_distance < key) 
		{ 
			arr[j + 1] = arr[j]; 
			j = j - 1; 
		}
		arr[j + 1].sprite_distance = key; 
	} 
} 

static void	init_sprites_info(t_vars *vars, t_sprites_sorter *sprites_srt)
{
	register int		i;

	i = -1;
	while (++i < vars->num_sprites)
		sprites_srt[i] = (t_sprites_sorter)
		{i, ((vars->cam.x - vars->sprites[i].x)
			* (vars->cam.x - vars->sprites[i].x)
			+ (vars->cam.y - vars->sprites[i].y)
			* (vars->cam.y - vars->sprites[i].y))
		};
	sort_sprites(vars->num_sprites, sprites_srt);

	/*for (size_t j = 0; j < vars->num_sprites; j++)
		printf("%lf ", sprites_srt[j].sprite_distance);
	printf("\n");debug sorting*/
}

void		cast_sprites(t_sprite *sprites, t_camera *cam, t_vars *vars)
{
	int					i;
	t_sprite			v_sprite;
	t_sprites_sorter	*sprites_srt;
	t_sprite_drawer		draw;
	t_texture			*text;
	
	if (!(sprites_srt = malloc(sizeof(t_sprites_sorter) * vars->num_sprites)))
		ft_putendl_fd("MALLOC ERROR", 2);
	init_sprites_info(vars, sprites_srt);
	i = -1;
	while (++i < vars->num_sprites)
	{
		v_sprite = sprites[sprites_srt[i].sprite_order];
		v_sprite.x -= cam->x;
		v_sprite.y -= cam->y;
		text = &(vars->text[v_sprite.tex_num]);

		draw.denom = 1.0 / (cam->plane.x * cam->dir_y - cam->dir_x * cam->plane.y);
		draw.transform_x = draw.denom * (cam->dir_y * v_sprite.x - cam->dir_x * v_sprite.y);
		draw.transform_y = draw.denom * (-cam->plane.y * v_sprite.x + cam->plane.x * v_sprite.y);
		
		draw.sprite_screen_x = (int)(WINDOW_WIDTH / 2) * (1 + draw.transform_x / draw.transform_y);
		
		//calculate height of the sprite on screen
		draw.sprite_height = fabs((int)WINDOW_HEIGHT / (draw.transform_y));
		//calculate lowest and highest pixel to fill in current stripe
		draw.start_y = -draw.sprite_height / 2 + WINDOW_HEIGHT / 2;
		if (draw.start_y < 0)
			draw.start_y = 0;
		draw.end_y = draw.sprite_height / 2 + WINDOW_HEIGHT / 2;
		if (draw.end_y >= WINDOW_HEIGHT)
			draw.end_y = WINDOW_HEIGHT - 1;
		
		//calculate width of the sprite
		draw.sprite_height = fabs((WINDOW_HEIGHT / (draw.transform_y)));
		draw.start_x = -draw.sprite_height / 2 + draw.sprite_screen_x;
		if (draw.start_x < 0)
			draw.start_x = 0;
		draw.end_x = draw.sprite_height / 2 + draw.sprite_screen_x;
		if (draw.end_x >= WINDOW_WIDTH)
			draw.end_x = WINDOW_WIDTH - 1;
	
		for(int stripe = draw.start_x; stripe < draw.end_x; stripe++)
		{
			int texX = (int)(256 * (stripe - (-draw.sprite_width / 2 + draw.sprite_screen_x)) * text->width / draw.sprite_width) / 256;
			//the conditions in the if are:
			//1) it's in front of camera plane so you don't see things behind you
			//2) it's on the screen (left)
			//3) it's on the screen (right)
			//4) vars->z_buffer, with perpendicular distance
		//	if (draw.transform_y > 0 && stripe > 0 && stripe < WINDOW_HEIGHT && draw.transform_y < vars->z_buffer[stripe])
			for(int y = draw.start_y; y < draw.end_y; y++) //for every pixel of the current stripe
			{
				int d = (y) * 256 - WINDOW_HEIGHT * 128 + draw.sprite_height * 128; //256 and 128 factors to avoid floats
				int texY = ((d * text->width) / draw.sprite_height) / 256;
				my_mlx_pixel_put(vars->img, stripe, y, text->array[text->width * texY + texX]);
			}
		}
	}
	free(sprites_srt);
}