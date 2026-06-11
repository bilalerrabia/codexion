/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_dongle_init.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/11 06:40:41 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	init_coders(t_params	*global)
{
	int	i;
	int	n;

	i = 0;
	n = global->coders_num;
	while (i < global->coders_num)
	{
		global->coders[i].compilation_number = 0;
		global->coders[i].global = global;
		global->coders[i].index = i + 1;
		global->coders[i].last_compilation = get_time_ms();
		global->coders[i].right_dongle = &global->dongles[i];
		global->coders[i].left_dongle = &global->dongles[(i + 1) % n];
		if (pthread_mutex_init(&global->coders[i].mutex, NULL))
			return (1);
		if (pthread_cond_init(&global->coders[i].wait_cond, NULL))
			return (1);
		i += 1;
	}
	return (0);
}

int	init_dongles(t_params	*global)
{
	int	i;
	int	n;

	i = 0;
	n = global->coders_num;
	while (i < n)
	{
		global->dongles[i].last_time_taken = 0;
		global->dongles[i].pq.size = 0;
		if (pthread_mutex_init(&global->dongles[i].mutex, NULL))
			return (1);
		global->dongles[i].taken = 0;
		i += 1;
	}
	return (0);
}
