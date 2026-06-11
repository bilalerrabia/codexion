/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/11 06:40:41 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static int	sim_stopped(t_params *global)
{
	int	stop;

	pthread_mutex_lock(&global->stop_mutex);
	stop = global->sim_stop;
	pthread_mutex_unlock(&global->stop_mutex);
	return (stop);
}

static int	take_dongles(t_coder *coder)
{
	t_dongle	*first;
	t_dongle	*second;

	if (coder->index % 2)
	{
		first = coder->left_dongle;
		second = coder->right_dongle;
	}
	else
	{
		first = coder->right_dongle;
		second = coder->left_dongle;
	}
	request_dongle(coder->global, coder, first);
	if (sim_stopped(coder->global))
		return (release_dongle(first), 1);
	request_dongle(coder->global, coder, second);
	if (sim_stopped(coder->global))
	{
		release_dongle(first);
		release_dongle(second);
		return (1);
	}
	return (0);
}

static int	compile_phase(t_coder *coder)
{
	pthread_mutex_lock(&coder->mutex);
	coder->last_compilation = get_time_ms();
	pthread_mutex_unlock(&coder->mutex);
	log_state(coder, "is compiling");
	usleep(1000 * coder->global->compiling_time);
	release_dongle(coder->right_dongle);
	release_dongle(coder->left_dongle);
	return (sim_stopped(coder->global));
}

static int	rest_phase(t_coder *coder)
{
	if (sim_stopped(coder->global))
		return (1);
	log_state(coder, "is debugging");
	usleep(1000 * coder->global->debugging_time);
	if (sim_stopped(coder->global))
		return (1);
	log_state(coder, "is refactoring");
	usleep(1000 * coder->global->refactoring_time);
	if (sim_stopped(coder->global))
		return (1);
	pthread_mutex_lock(&coder->mutex);
	coder->compilation_number += 1;
	pthread_mutex_unlock(&coder->mutex);
	return (0);
}

void	*coder_func(void *args)
{
	t_coder		*coder;
	t_params	*global;
	int			comp_num;

	coder = (t_coder *)args;
	global = coder->global;
	pthread_mutex_lock(&global->stop_mutex);
	while (!global->all_ready)
		pthread_cond_wait(&global->cond, &global->stop_mutex);
	pthread_mutex_unlock(&global->stop_mutex);
	if (coder->index % 2 == 0)
		usleep(400);
	comp_num = 0;
	while (!sim_stopped(global) && comp_num < global->required_compile)
	{
		if (take_dongles(coder))
			return (NULL);
		if (compile_phase(coder))
			return (NULL);
		if (rest_phase(coder))
			return (NULL);
		comp_num += 1;
	}
	return (NULL);
}
