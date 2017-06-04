/*
 * Simple TCP congestion control module for Linux based on RFC 5681.
 * 
 * Copyright 2017 Anmol Sarma <me@anmolsarma.in>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <linux/mm.h>
#include <linux/module.h>
#include <linux/math64.h>
#include <net/tcp.h>

void neoreno_cong_avoid(struct sock *sk, u32 ack, u32 acked)
{
	struct tcp_sock *tp = tcp_sk(sk);

	if (tcp_in_slow_start(tp))
		tp->snd_cwnd = min(tp->snd_cwnd + acked, tp->snd_ssthresh);

	/* Congestion avoidance, increment cwnd for each cwnd of data ACKed */
	else {
		tp->snd_cwnd_cnt += acked;
		if (tp->snd_cwnd_cnt >= tp->snd_cwnd) {
			tp->snd_cwnd_cnt = 0;
			tp->snd_cwnd++;
		}
	}
}

u32 neoreno_ssthresh(struct sock *sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	/* RFC 5681 Equation (4) */
	return max(tp->packets_out >> 1U, 2U);
}

u32 neoreno_undo_cwnd(struct sock * sk)
{
	struct tcp_sock *tp = tcp_sk(sk);

	return max(tp->snd_cwnd, tp->snd_ssthresh << 1);
}

struct tcp_congestion_ops neoreno = {
	.flags = TCP_CONG_NON_RESTRICTED,
	.name = "neoreno",
	.owner = THIS_MODULE,
	.ssthresh = neoreno_ssthresh,
	.cong_avoid = neoreno_cong_avoid,
	.undo_cwnd = neoreno_undo_cwnd,
};

static int __init neoreno_register(void)
{
	return tcp_register_congestion_control(&neoreno);
}

static void __exit neoreno_unregister(void)
{
	tcp_unregister_congestion_control(&neoreno);
}

module_init(neoreno_register);
module_exit(neoreno_unregister);

MODULE_AUTHOR("Anmol Sarma");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Neo-Reno TCP");
