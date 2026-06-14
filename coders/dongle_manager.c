/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_manager.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/13 19:04:07 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	release_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->taken = 0;
	dongle->last_time_taken = get_time_ms();
	pthread_mutex_unlock(&dongle->mutex);
}

long	dongle_key(t_params *global, t_coder *coder)
{
	long	last;

	if (strcmp(global->scheduler, "fifo") == 0)
		return (get_time_ms());
	pthread_mutex_lock(&coder->mutex);
	last = coder->last_compilation;
	pthread_mutex_unlock(&coder->mutex);
	return (last + global->burnout_time);
}

int	dongle_free(t_params *global, t_coder *coder, t_dongle *dongle)
{
	long	rest_time;

	rest_time = get_time_ms() - dongle->last_time_taken;
	return (dongle->pq.size > 0 && !dongle->taken
		&& global->dongle_cooldown <= rest_time
		&& dongle->pq.nodes[0].coder->index == coder->index);
}

void	wait_5ms(t_coder *coder, t_dongle *dongle)
{
	long			wakeup;
	struct timespec	ts;

	wakeup = get_time_ms() + 5;
	ts.tv_sec = wakeup / 1000;
	ts.tv_nsec = (wakeup % 1000) * 1000000;
	pthread_cond_timedwait(&coder->wait_cond, &dongle->mutex, &ts);
}

int	request_dongle(t_params *global, t_coder *coder, t_dongle *dongle)
{
	int	stop;

	pthread_mutex_lock(&dongle->mutex);
	pq_push(&dongle->pq, coder, dongle_key(global, coder));
	while (1)
	{
		pthread_mutex_lock(&global->stop_mutex);
		stop = global->sim_stop;
		pthread_mutex_unlock(&global->stop_mutex);
		if (stop)
			return (pthread_mutex_unlock(&dongle->mutex), 0);
		if (dongle_free(global, coder, dongle))
			break ;
		wait_5ms(coder, dongle);
	}
	dongle->taken = 1;
	pq_pop(&dongle->pq);
	pthread_mutex_unlock(&dongle->mutex);
	log_state(coder, "has taken a dongle");
	return (1);
}
