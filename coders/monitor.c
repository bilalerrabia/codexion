/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/12 18:08:25 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static int	coder_status(t_params *global, t_coder *coder)
{
	long	last;
	int		comp_num;

	pthread_mutex_lock(&coder->mutex);
	last = coder->last_compilation;
	comp_num = coder->compilation_number;
	pthread_mutex_unlock(&coder->mutex);
	if (comp_num < global->required_compile
		&& last + global->burnout_time <= get_time_ms())
		return (-1);
	return (comp_num >= global->required_compile);
}

static int	check_coders(t_params *global)
{
	int	i;
	int	count;
	int	status;

	i = 0;
	count = 0;
	while (i < global->coders_num)
	{
		status = coder_status(global, &global->coders[i]);
		if (status == -1)
		{
			global->sim_stop = 1;
			log_state(&global->coders[i], "burned out");
			return (1);
		}
		count += status;
		i += 1;
	}
	if (count == global->coders_num)
		global->sim_stop = 1;
	return (count == global->coders_num);
}

void	*monitor_func(void *args)
{
	t_params	*global;
	int			done;

	global = (t_params *)args;
	while (1)
	{
		pthread_mutex_lock(&global->stop_mutex);
		done = check_coders(global);
		pthread_mutex_unlock(&global->stop_mutex);
		if (done)
			return (NULL);
		usleep(1000);
	}
	return (NULL);
}
