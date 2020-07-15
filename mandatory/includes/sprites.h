/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprites.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dirty <dirty@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/18 14:34:19 by smaccary          #+#    #+#             */
/*   Updated: 2020/07/15 00:03:47 by dirty            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SPRITES_H
# define SPRITES_H

# include "structs/structs.h"
# include "settings.h"
# include "frontend.h"

void	sort_sprites(t_camera *cam, t_sprite *arr, int n) __attribute__((hot));
void	init_sprites_info(t_camera *cam, t_sprite *sprites, int n)\
 __attribute__((hot));
void	init_sprite_drawing(t_sprite_drawer *d, t_camera *c,
				t_sprite *s, t_texture *t) __attribute__((hot));

#endif
