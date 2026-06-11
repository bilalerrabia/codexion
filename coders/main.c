/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:46:17 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/11 06:47:27 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	main(int ac,	char *av[])
{
	t_params	global;

	if (ac != 9)
	{
		printf("invalid parameters\n");
		return (1);
	}
	if (parser(&global, av + 1))
	{
		return (1);
	}
	if (init_simulation(&global))
		ft_exit(&global);
	if (start_simulation(&global))
		ft_exit(&global);
	ft_exit(&global);
	return (0);
}

void	ft_exit(t_params	*global)
{
	int	i;

	i = 0;
	if (global)
	{
		while (i < global->coders_num)
		{
			pthread_mutex_destroy(&global->coders[i].mutex);
			pthread_mutex_destroy(&global->dongles[i].mutex);
			pthread_cond_destroy(&global->coders[i].wait_cond);
			i += 1;
		}
		pthread_mutex_destroy(&global->stop_mutex);
		pthread_mutex_destroy(&global->print_mutex);
		pthread_mutex_destroy(&global->start_mutex);
		pthread_cond_destroy(&global->cond);
		free(global->coders);
		free(global->dongles);
	}
	exit(0);
}
