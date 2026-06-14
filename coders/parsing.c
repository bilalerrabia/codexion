/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:21:53 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/13 18:44:04 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

int	min_atoi(char	*str)
{
	int	i;

	if (!str || !str[0])
		return (-1);
	i = 0;
	while (str[i])
	{
		if (str[i] > '9' || str[i] < '0')
			return (-1);
		i++;
	}
	return (atoi(str));
}

int	parser(t_params	*global, char	*av[])
{
	global->coders_num = min_atoi(av[0]);
	global->burnout_time = min_atoi(av[1]);
	global->compiling_time = min_atoi(av[2]);
	global->debugging_time = min_atoi(av[3]);
	global->refactoring_time = min_atoi(av[4]);
	global->required_compile = min_atoi(av[5]);
	global->dongle_cooldown = min_atoi(av[6]);
	global->scheduler = av[7];
	if (strcmp(av[7], "fifo") != 0 && strcmp(av[7], "edf") != 0)
		return (1);
	return (checker(global));
}

int	checker(t_params	*global)
{
	if (
		global->coders_num <= 0 || global->burnout_time < 0
		|| global->compiling_time < 0 || global->debugging_time < 0
		|| global->refactoring_time < 0 || global->required_compile <= 0
		|| global->dongle_cooldown < 0
	)
		return (1);
	return (0);
}
