/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/13 19:15:06 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
# define HEADER_H

# include <pthread.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <sys/time.h>
# include <time.h>
# include <string.h>

typedef struct s_coder	t_coder;

typedef struct s_pq_node
{
	t_coder	*coder;
	long	key;
}	t_pq_node;

typedef struct s_pqueue
{
	t_pq_node	nodes[2];
	int			size;
}	t_pqueue;

typedef struct s_dongle
{
	int				taken;
	long			last_time_taken;
	pthread_mutex_t	mutex;
	t_pqueue		pq;
}	t_dongle;

typedef struct s_params
{
	int				coders_num;
	long			burnout_time;
	long			compiling_time;
	long			debugging_time;
	long			refactoring_time;
	int				required_compile;
	long			dongle_cooldown;
	long			start_time;
	int				all_ready;
	char			*scheduler;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_cond_t	cond;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	start_mutex;
	pthread_mutex_t	stop_mutex;
	int				sim_stop;
}	t_params;

struct s_coder
{
	int				index;
	long			last_compilation;
	int				compilation_number;
	pthread_mutex_t	mutex;
	pthread_cond_t	wait_cond;
	pthread_t		thread;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	t_params		*global;
};

void		ft_swap(t_pq_node *a, t_pq_node *b);
void		sift_up(t_pqueue *queue, int i);
t_pq_node	pq_pop(t_pqueue *pq);
void		pq_push(t_pqueue *pq, t_coder *coder, long key);
int			checker(t_params *global);
int			parser(t_params *global, char *av[]);
int			min_atoi(char *str);
long		get_time_ms(void);
void		log_state(t_coder *coder, char *msg);
int			init_simulation(t_params *global);
int			start_simulation(t_params *global);
void		ft_exit(t_params *global);
void		*coder_func(void *args);
void		*monitor_func(void *args);
int			request_dongle(t_params *global, t_coder *coder, t_dongle *dongle);
void		release_dongle(t_dongle *dongle);
int			init_coders(t_params *global);
int			init_dongles(t_params *global);
int			smart_sleep(long time, t_params *global);
long		dongle_key(t_params *global, t_coder *coder);
int			dongle_free(t_params *global, t_coder *coder, t_dongle *dongle);
void		wait_5ms(t_coder *coder, t_dongle *dongle);
int			compile_phase(t_coder *coder);
int			take_dongles(t_coder *coder);
int			debug_refact_phase(t_coder *coder);
int			check_coders(t_params *global);
int			coder_status(t_params *global, t_coder *coder);

#endif
