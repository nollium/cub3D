/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   colors_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smaccary <smaccary@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/01 12:58:15 by smaccary          #+#    #+#             */
/*   Updated: 2020/05/01 15:50:22 by smaccary         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3D.h"

int add_shade(double shade, int color)
{
	int i;
	int trgb[4];
	static time_t warning_count = 0;

	if (0 > shade || shade > 1)
	{
		if (WARNINGS && !(++warning_count % WARN_LEVEL))
			printf("\e[1;35m Warning : \e[0m add_shade should only use a shade between 0 and 1 (%F).\n", shade * WINDOW_WIDTH / 2);
		shade = (shade < 0) ? 0 : 1;
	}
	trgb[0] = color & 0xFF;
	trgb[1] = (color >> 8) & 0xFF;
	trgb[2] = (color >> 16) & 0xFF;
	trgb[3] = (color >> 24) & 0xFF;
	i = -1;
	while (++i < 3)
		trgb[i] = trgb[i] - shade * trgb[i];
  	return(trgb[3] << 24 | trgb[2] << 16 | trgb[1] << 8 | trgb[0]);
}

/*
// weird transparency stuff, works for water / fog effect
int add_shade(double dist, int color)
{
	int i;
	int trgb[4];

	if (0 > dist || dist > 1)
	{
		dist = (dist < 0) ? 0 : 1;
		ft_putstr_fd("\e[1;35m Warning : \e[0m add_shade should only use a shade between 0 and 1.\n", 1);
	}
	trgb[0] = color & 0xFF;
	trgb[1] = (color >> 8) & 0xFF;
	trgb[2] = (color >> 16) & 0xFF;
	trgb[3] = (color >> 24) & 0xFF;
	i = -1;
	while (++i < 3)
		trgb[i] = trgb[i] - dist * trgb[i];
  	return(trgb[3] << 24 | trgb[2] << 16 | trgb[1] << 8 | trgb[0]);
}
*/