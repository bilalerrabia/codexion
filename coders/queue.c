/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   queue.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: berrabia <berrabia@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/11 06:20:10 by berrabia          #+#    #+#             */
/*   Updated: 2026/06/11 06:40:41 by berrabia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void	ft_swap(t_pq_node *a, t_pq_node *b)
{
	t_pq_node	swap;

	swap = *a;
	*a = *b;
	*b = swap;
}

void	sift_up(t_pqueue *queue, int i)
{
	while (i > 0)
	{
		if (queue->nodes[i].key < queue->nodes[(i - 1) / 2].key)
		{
			ft_swap(&queue->nodes[i], &queue->nodes[(i - 1) / 2]);
			i = (i - 1) / 2;
		}
		else
			break ;
	}
}

t_pq_node	pq_pop(t_pqueue *pq)
{
	t_pq_node	tmp;

	if (pq->size < 1)
	{
		tmp.coder = NULL;
		tmp.key = -1;
		return (tmp);
	}
	if (pq->size == 1)
	{
		pq->size = 0;
		return (pq->nodes[0]);
	}
	ft_swap(&pq->nodes[0], &pq->nodes[1]);
	pq->size = 1;
	return (pq->nodes[1]);
}

void	pq_push(t_pqueue *pq, t_coder *coder, long key)
{
	pq->nodes[pq->size].coder = coder;
	pq->nodes[pq->size].key = key;
	pq->size += 1;
	sift_up(pq, pq->size - 1);
}
