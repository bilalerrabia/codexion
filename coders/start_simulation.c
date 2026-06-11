/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_simulation.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/11 06:40:41 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	init_simulation(t_params *global)
{
	global->sim_stop = 0;
	global->coders = malloc(sizeof(t_coder) * global->coders_num);
	if (!global->coders)
		return (1);
	global->dongles = malloc(sizeof(t_dongle) * global->coders_num);
	if (!global->dongles)
		return (free(global->coders), 1);
	pthread_mutex_init(&global->start_mutex, NULL);
	pthread_mutex_init(&global->stop_mutex, NULL);
	pthread_mutex_init(&global->print_mutex, NULL);
	pthread_cond_init(&global->cond, NULL);
	if (init_coders(global))
		ft_exit(global);
	if (init_dongles(global))
		ft_exit(global);
	global->start_time = get_time_ms();
	return (0);
}

int	start_simulation(t_params *global)
{
	int			i;
	pthread_t	monitor;

	global->all_ready = 0;
	i = 0;
	while (i < global->coders_num)
	{
		pthread_create(&global->coders[i].thread, NULL,
			coder_func, &global->coders[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_func, global);
	pthread_mutex_lock(&global->stop_mutex);
	global->all_ready = 1;
	pthread_cond_broadcast(&global->cond);
	pthread_mutex_unlock(&global->stop_mutex);
	i = 0;
	while (i < global->coders_num)
	{
		pthread_join(global->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	return (0);
}
