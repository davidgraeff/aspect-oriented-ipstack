$VAR1 = {
	'vname' => 'IP Stack',
	'name' => 'ipstack',
	'dir' => 'src/ipstack',
	'depends' => '&ipstack',
	'comp' => [
	{
		'file' => 'IPStack_Config.h',
		'srcfile' => 'IPStack_Config_kconf.h'
	},
	{
		'files' => ['ip/InternetChecksum.h','Socket_Send.ah','Socket_Send_Slice.ah','SendBuffer.h','ip/SendBuffer_IP.ah','ip/SendBuffer_IP_Slice.ah']
	},
	{
		'depends' => '&ipstack_debug_failed_checksums',
		'files' => ['ip/InternetChecksum_Debug.ah']
	},
	{
		'depends' => '&ipstack_debug_sendbuffer',
		'files' => ['Sendbuffer_Debug.ah']
	},
	{
		'depends' => '&ipstack_debug_sendbuffer or &ipstack_protect_sendbuffer',
		'files' => ['Sendbuffer_DebugInfo.ah', 'Sendbuffer_DebugInfo_Slice.ah']
	},
	{
		'depends' => '&ipstack_protect_sendbuffer',
		'files' => ['Sendbuffer_Protect.ah', 'Sendbuffer_Protect_Slice.ah']
	},
	{
		'vname' => 'Router',
		'name' => 'ipstack_router',
		'subdir' => '/router',
		'files' => [
			'Interface.h',
			'Interface.ah',
			'Interface_Delegation.ah',
			'Router.h',
			'Router.cpp',
			'Router.ah'
		]
	},
	{
		'vname' => 'IP Management Buffer',
		'name' => 'ipstack_router',
		'depends' => '&ipstack_icmp_ipv4 or &ipstack_icmp_ipv6 or &ipstack_tcp_reset',
		'subdir' => '/ip_management_memory',
		'comp' => [
			{
				'files' => ['ManagementMemory.cpp', 'ManagementMemory.h', 'SharedMemory.h', 'SharedMemory.cpp', 'SharedMemory.ah']
			},
			{
				'depends' => '&ipstack_debug_management_memory',
				'files' => ['debug_management_memory.ah']
			}
		]
	},
	{
		'depends' => '&ipstack_linklayer_offloading_tx_icmpv4',
		'files' => ['ip/InternetChecksum_Tx_Offloading_ICMPv4.ah']
	},
	{
		'depends' => '&ipstack_linklayer_offloading_tx_tcp',
		'files' => ['ip/InternetChecksum_Tx_Offloading_TCP.ah']
	},
	{
		'depends' => '&ipstack_linklayer_offloading_tx_udp',
		'files' => ['ip/InternetChecksum_Tx_Offloading_UDP.ah']
	},
	{
		'depends' => '&ipstack_linklayer_offloading_rx',
		'files' => ['ip/InternetChecksum_Rx_Offloading.ah']
	},
	{
		'depends' => '&ipstack_debug_incoming',
		'files' => ['ipstack_debug_incoming.ah']
	},
	{
		'vname' => 'Receive',
		'name' => 'ipstack_receive',
		'depends' => '&ipstack_ipv4_recv or &ipstack_ipv6',
		'comp' => [
		{
			'files' => ['ReceiveBuffer.h']
		},
		{
			'vname' => 'Demux',
			'name' => 'ipstack_demux',
			'subdir' => '/demux',
			'files' => [
				'Demux.h',
				'Demux.cpp',
				'Demux.ah',
				'DemuxLinkedListContainer.h'
			]
		}
		]
	},
	{
		'vname' => 'Util',
		'name' => 'ipstack_util',
		'subdir' => '/util',
		'comp' => [
		{
			'files' => [
				'Mempool.h',
				'MempoolBase.h',
				'Ringbuffer.h',
				'RingbufferBase.h'
			]
		}
		]
	},
	{
		'depends' => '&ipstack_byteorder_little',
		'file' => 'Little_Endian.ah'
	},
	{
		'depends' => '&ipstack_debug_protected_receive',
		'file' => 'Protect_Receive_Debug.ah'
	},
	{
		'vname' => 'Autosar OS Integration',
		'name' => 'ipstack_autosar',
		'subdir' => '/as',
		'comp' => [
		{
			'file' => 'SystemHalt.h'
		},
		{
			'depends' => '&ipstack_irqsync',
			'file' => 'IRQ_Sync.ah'
		},
		{
			'depends' => '&ipstack_clock',
			'file' => 'Clock.h',
		},
		{
			'depends' => '&ipstack_autosar_event',
			'files' => [
				'EventSupport.ah',
				'EventSupport.h',
				'EventSupport_Slice.ah'
			]
		},
		{
			'depends' => '&ipstack_autosar_alarm and &ipstack_autosar_event',
			'files' => [
				'AlarmSupport.ah',
				'AlarmSupport_Slice.ah',
			]
		},
		{
			'depends' => '&ipstack_autosar_reschedule_address_resolution',
			'file' => 'AddressResolutionReSchedule.ah'
		},
		{
			'depends' => '&ipstack_autosar_reschedule_sendbuffer_free',
			'file' => 'Sendbuffer_free_Reschedule.ah'
		}
		]
	},
	{
		'depends' => '&ipstack_linklayer_ethernet',
		'subdir' => '/ethernet',
		'comp' => [
		{
			'files' => ['Eth_Frame.h','Receive_Ethernet.ah','Demux_Ethernet_Slice.ah','EthernetAddressUtilities.cpp','EthernetAddressUtilities.h','Sendbuffer_Ethernet.ah','Sendbuffer_Ethernet_Slice.ah']
		}
		]
	},
	{
		'depends' => '&ipstack_linklayer_direct_response',
		'comp' => [
		{
			'files' => ['demux/DirectResponse.ah','demux/DirectResponse_Slice.ah']
		},
		{
			'depends' => '&ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'files' => ['Sendbuffer_DirectResponse_Ethernet.ah','DirectResponse_ethernet.ah']
		}
		]
	},
	{
		'vname' => '6lowpan IPv6 headercompression',
		'name' => 'ipstack_ipv6_headercompression',
		'depends' => '&ipstack_ipv6_headercompression',
		'subdir' => '/6lowpan',
		'comp' => [
		{
			'files' => ['IPv6hc.ah','IPv6iphc.h','IPv6iphc.cpp']
		},
		{
			'depends' => '&ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'files' => ['IPv6_iphc_Addresses_from_Ethernet.ah','IPv6_iphc_Receive_Ethernet.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_destaddr_prefix_use0',
			'files' => ['IPv6hc_dest_addr_0bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_destaddr_prefix_use16',
			'files' => ['IPv6hc_dest_addr_2bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_destaddr_prefix_use64',
			'files' => ['IPv6hc_dest_addr_8bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_multicast_use8',
			'files' => ['IPv6hc_dest_multicast_addr_1bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_multicast_use32',
			'files' => ['IPv6hc_dest_multicast_addr_2bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_multicast_use48',
			'files' => ['IPv6hc_dest_multicast_addr_4bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_sourceaddr_prefix_use0',
			'files' => ['IPv6hc_src_addr_0bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_sourceaddr_prefix_use16',
			'files' => ['IPv6hc_src_addr_2bytes.ah']
		},
		{
			'depends' => '&ipstack_ipv6_hc_sourceaddr_prefix_use64',
			'files' => ['IPv6hc_src_addr_8bytes.ah']
		}
		]
		
	},
	{
		'vname' => 'Dualstack IPv4 and IPv6',
		'name' => 'ipstack_ipv4_and_ipv6',
		'depends' => '&ipstack_ipv4 and &ipstack_ipv6',
		'subdir' => '/ip',
		'comp' => [
		{
			'files' => ['IP_Dualstack_IPVx_Slice.ah','IP_Dualstack_Slice.ah','IP_Dualstack_Socket_Delegation.ah']
		}
		]
	},
	{
		'vname' => 'Singlestack IPv4',
		'name' => 'ipstack_ipv4_single',
		'depends' => '&ipstack_ipv4 and not &ipstack_ipv6',
		'subdir' => '/ip',
		'comp' => [
		{
			'files' => ['IPv4_Singlestack_Slice.ah','IPv4_Socket_Delegation.ah']
		}
		]
	},
	{
		'vname' => 'Singlestack IPv6',
		'name' => 'ipstack_ipv6_single',
		'depends' => '&ipstack_ipv6 and not &ipstack_ipv4',
		'subdir' => '/ip',
		'comp' => [
		{
			'files' => ['IPv6_Singlestack_Slice.ah','IPv6_Socket_Delegation.ah']
		}
		]
	},
	{
		'vname' => 'IPv4',
		'name' => 'ipstack_ipv4',
		'depends' => '&ipstack_ipv4',
		'subdir' => '/ipv4',
		'comp' => [
		{
			'files' => ['IPv4.ah', 'IPv4.h', 'IPv4onSockets.h', 'IPv4onSockets.cpp', 'Interface_IPv4_Slice.ah', 'Router_IPv4_Slice.ah', 'SendBuffer_IPv4.ah', 'SendBuffer_IPv4_checksum.ah', 'Socket_IPv4_Slice.ah']
		},
		{
			'vname' => 'IPv4 Receiving',
			'name' => 'ipstack_ipv4_recv',
			'depends' => '&ipstack_ipv4_recv',
			'comp' => [
			{
				'files' => ['Demux_IPv4_Slice.ah', 'IPv4_Receive.ah', 'IPv4_Receive_Raw.ah']
			},
			{
				'depends' => '&ipstack_ipv4_recv_fragments',
				'subdir' => '/fragmentation',
				'file' => 'IPv4_Receive_Fragment_Reassembly.ah'
			}
			]
		},
		{
			'depends' => '&ipstack_ipv4_recv and &ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'file' => 'IPv4_Receive_Ethernet.ah'
		},
		{
			'depends' => '&ipstack_ipv4_send and &ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'files' => ['Send_IPv4.ah']
		},
		{
			'depends' => '&ipstack_ipv4_fwd and &ipstack_ipv4_recv',
			'file' => 'IPv4_Forward.ah'
		},
		{
			'vname' => 'Static IPv4 Configuration of Interface 0',
			'depends' => '&ipstack_ifcfg0',
			'subdir' => '/static',
			'file'    => 'IPv4_StaticInterface0_Configuration.ah', 
		},
		{
			'vname' => 'UDP over IPv4',
			'name' => 'ipstack_udp_ipv4',
			'depends' => '&ipstack_udp',
			'subdir' => '/udp',
			'comp' => [
			{
				'vname' => 'UDP over IPv4 - Sending',
				'name' => 'ipstack_udp_send',
				'depends' => '&ipstack_udp_send',
				'comp' => [
				{
					'depends' => '&ipstack_udp_send_checksum',
					'file' => 'IPv4_UDP_Tx_Checksumming.ah'
				}
				]
			},
			{
				'vname' => 'UDP over IPv4 - Receiving',
				'name' => 'ipstack_udp_recv',
				'depends' => '&ipstack_udp_recv',
				'comp' => [
				{
					'files' => ['Demux_UDP_IPv4_Slice.ah', 'ReceiveBufferUDPIPv4.h', 'Receive_UDP_IPv4.ah']
				},
				{
					'depends' => '&ipstack_udp_recv_checksum',
					'file' => 'IPv4_UDP_Rx_Checksumming.ah'
				}
				]
			}
			]
		},
		{
			'vname' => 'Internet Checksum v4',
			'name' => 'ipstack_inetchecksum_ipv4',
			'depends' => '&ipstack_icmp_ipv4 or &ipstack_udp_recv_checksum or &ipstack_udp_send_checksum or &ipstack_tcp',
			'comp' => [
			{
				'file' => 'InternetChecksum.h'
			},
			{
				'depends' => '&ipstack_linklayer_offloading_rx'
			}
			]
		},
		{
			'vname' => 'TCP over IPv4',
			'name' => 'ipstack_tcp_ipv4',
			'depends' => '&ipstack_tcp and &ipstack_ipv4',
			'subdir' => '/tcp',
			'comp' => [
			{
				'files' => [
					'IPv4_TCP_Receive.ah',
					'IPv4_TCP_Tx_Checksumming.ah'
				]
			},
			{
				'vname' => 'Listen Sockets',
				'name' => 'ipstack_tcp_listen',
				'depends' => '&ipstack_tcp_listen',
				'files' => [
					'IPv4_TCP_Listen.ah'
				]
			},
			{
				'vname' => 'Reset invalid Packets',
				'name' => 'ipstack_tcp_reset',
				'depends' => '&ipstack_tcp_reset',
				'comp' => [
				{
					'files' => [
						'IPv4_TCP_Reset.ah'
					]
				}
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'IPv6',
		'name' => 'ipstack_ipv6',
		'depends' => '&ipstack_ipv6',
		'subdir' => '/ipv6',
		'comp' => [
		{
			'files' => ['AddressMemory.h', 'Demux_IPv6_Slice.ah', 'ExtensionHeaderOptions.h', 'IPv6.ah', 'IPv6.h', 'IPv6AddressUtilities.cpp', 'IPv6AddressUtilities.h', 'IPv6_Receive.ah', 'IPv6_Receive_Raw.ah', 'IPv6_addlinklocalIP.ah', 'IPv6onSockets.h', 'IPv6onSockets.cpp', 'Interface_IPv6_Slice.ah', 'RouteResult.h', 'Router_IPv6_Slice.ah', 'SendBuffer_IPv6.ah', 'Socket_IPv6_Slice.ah']
		},
		{
			'vname' => 'Always copy this even if no ndpcache. we need it for addressmemory entry size determination',
			'files' => ['ndpcache/NDPCacheEntry.h']
		},
		{
			'depends' => '&ipstack_ndpcache_ipv6_request_and_store',
			'subdir' => '/ndpcache',
			'comp' => [
				{
					'files' => ['NDPCache_Interface_Slice.ah', 'NDPCache.ah', 'NDPCacheConfig.h']
				},
				{
					'depends' => '&ipstack_ipv6_ndpcache_debug',
					'file' => 'NDPCacheDebug.ah'
				},
				{
					'depends' => '&ipstack_ipv6_ndplookup_debug',
					'file' => 'NDPLookupDebug.ah'
				},
				{
					'depends' => '&ipstack_ndpcache_ipv6_async',
					'file' => 'NDPCacheEntry_Asynchronous_Slice.ah'
				}
			]
		},
		{
			'depends' => '&ipstack_ndpcache_ipv6_expire',
			'subdir' => '/ndpcache_expire',
			'comp' => [
				{
					'files' => ['Interface_Expire_Slice.ah', 'NDPCacheEntry_Expire_Slice.ah', 'NDPCacheExpire.ah', 'NDPCacheExpireConfig.h']
				},
				{
					'depends' => '&ipstack_ipv6_ndcache_expire_debug',
					'file' => 'ExpireDebug.ah'
				}
			]
		},
		{
			'vname' => 'IPv6 Receicing with ethernet',
			'depends' => '&ipstack_ipv6 and &ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'comp' => [
				{
					'files' => ['Receive_IPv6.ah']
				},
				{
					'vname' => 'Cache incoming mac addresses in the ndp cache',
					'depends' => '&ipstack_ndpcache_ipv6_cacheincoming',
					'file' => 'Cache_Incoming_NDPCache.ah'
				},
				{
					'depends' => '&ipstack_ndpcache_ipv6_request_and_store',
					'files' => ['NDPCacheEntry_Ethernet.ah', 'NDPCacheEntry_Ethernet_Slice.ah', 'NDPCache_BlockingLookup.ah']
				}
			]
		},
		{
			'vname' => 'IPv6 Sending with ethernet',
			'depends' => '&ipstack_ipv6 and &ipstack_linklayer_ethernet',
			'subdir' => '/ethernet',
			'files' => ['Send_IPv6.ah']
		},
		{
			'vname' => 'IPv6 ethernet no privacy extension',
			'depends' => '&ipstack_ipv6 and &ipstack_linklayer_ethernet and not &ipstack_ipv6_privacy_extension',
			'files' => ['IPv6AddressSuffixEthernet.ah']
		},
		{
			'vname' => 'IPv6 ethernet privacy extension',
			'depends' => '&ipstack_ipv6 and &ipstack_linklayer_ethernet and &ipstack_ipv6_privacy_extension',
			'files' => ['IPv6AddressSuffixPrivacyExtension.ah']
		},
		{
			'vname' => 'Multicast_ipv6',
			'name' => 'ipstack_multicast_ipv6',
			'subdir' => '/multicast',
			'depends' => '&ipstack_multicast_ipv6',
			'files' => ['IPv6_Multicast.ah', 'Interface_Multicast_Slice.ah', 'MulticastEntry.h']
		},
		{
			'vname' => 'ipv6_hopbyhop',
			'name' => 'ipstack_ipv6_hopbyhop',
			'subdir' => '/ipv6_hopbyhopheader',
			'depends' => '&ipstack_ipv6_hopbyhop',
			'comp' => [
			{
				'files' => ['IPv6HopByHop.h', 'SendBuffer_HopByHop_Extension.ah', 'hopbyhop_Receive.ah']
			},
			{
				'vname' => 'Altert Option',
				'name' => 'ipv6_hopbyhop_alert',
				'subdir' => '/option_alert',
				'depends' => '&ipstack_ipv6_hopbyhop_alert',
				'files' => ['hopbyhop_alertMessage.ah', 'hopbyhop_alertMessage_IPv6_Slice.ah']
			}
			]
			
		},
		{
			'vname' => 'ipstack_ipv6_destination',
			'name' => 'ipstack_ipv6_destination',
			'subdir' => '/ipv6_destinationheader',
			'depends' => '&ipstack_ipv6_destination',
			'files' => ['receive.ah']
		},
		{
			'vname' => 'ipstack_ipv6_fragmentation_reassembly',
			'name' => 'ipstack_ipv6_fragmentation_reassembly',
			'subdir' => '/ipv6_fragmentationheader',
			'depends' => '&ipstack_ipv6_fragmentation_reassembly',
			'files' => ['receive.ah']
		},
		{
			'subdir' => '/ipv6_authheader',
			'depends' => '&ipstack_ipv6_authheader',
			'files' => ['receive.ah']
		},
		{
			'subdir' => '/ipv6_espheader',
			'depends' => '&ipstack_ipv6_espheader',
			'files' => ['receive.ah']
		},
		{
			'subdir' => '/ipv6_mobileheader',
			'depends' => '&ipstack_ipv6_mobileheader',
			'files' => ['receive.ah']
		},
		{
			'subdir' => '/ipv6_routerheader',
			'depends' => '&ipstack_ipv6_routerheader',
			'files' => ['receive.ah']
		},
		{
			'subdir' => '/address_prefix_expire',
			'depends' => '&ipstack_ipv6_address_expire',
			'files' => ['Address_prefix_expire.ah', 'Address_prefix_expire_Slice.ah', 'Interface_prefix_expire_Slice.ah']
		},
		{
			'subdir' => '/destcache',
			'depends' => '&ipstack_ipv6_dest_cache',
			'files' => ['Destination_Cache.ah', 'IPV6_Destination_Cache_Slice.ah', 'Router_Destination_Cache_Slice.ah']
		},
		{
			'subdir' => '/destcache',
			'depends' => '&ipstack_ipv6_dest_cache_debug',
			'files' => ['Destination_Cache_Debug.ah']
		},
		{
			'vname' => 'Privacy extension',
			'depends' => '&ipstack_ipv6_privacy_extension',
			'file'    => 'IPv6AddressSuffixPrivacyExtension.ah', 
		},
		{
			'vname' => 'Static IPv6 Configuration of Interface 0',
			'depends' => '&ipstack_ifcfg0v6ip',
			'subdir' => '/static',
			'file'    => 'IPv6_static_interface_ip.ah', 
		},
		{
			'vname' => 'Static IPv6 Router Configuration of Interface 0',
			'depends' => '&ipstack_ifcfg0v6router',
			'subdir' => '/static',
			'file'    => 'IPv6_static_router_entry.ah', 
		},
		{
			'vname' => 'Static IPv6 Prefix Configuration of Interface 0',
			'depends' => '&ipstack_ifcfg0v6routerprefix',
			'subdir' => '/static',
			'file'    => 'IPv6_static_router_prefix.ah', 
		},
		{
			'vname' => 'UDP over IPv6',
			'name' => 'ipstack_udp_ipv6',
			'depends' => '&ipstack_udp',
			'subdir' => '/udp',
			'comp' => [
			{
				'vname' => 'UDP over IPv6 - Sending',
				'name' => 'ipstack_udp_send',
				'depends' => '&ipstack_udp_send',
				'comp' => [
				{
					'depends' => '&ipstack_udp_send_checksum',
					'file' => 'IPv6_UDP_Tx_Checksumming.ah'
				}
				]
			},
			{
				'vname' => 'UDP over IPv6 - Receiving',
				'name' => 'ipstack_udp_recv',
				'depends' => '&ipstack_udp_recv',
				'comp' => [
				{
					'files' => ['Demux_UDP_IPv6_Slice.ah', 'ReceiveBufferUDPIPv6.h', 'Receive_UDP_IPv6.ah']
				},
				{
					'depends' => '&ipstack_udp_recv_checksum',
					'file' => 'IPv6_UDP_Rx_Checksumming.ah'
				}
				]
			}
			]
		},
		{
			'vname' => 'Internet Checksum v6',
			'name' => 'ipstack_inetchecksum_ipv6',
			'depends' => '&ipstack_icmp_ipv6 or &ipstack_udp_recv_checksum or &ipstack_udp_send_checksum or &ipstack_tcp',
			'comp' => [
			{
				'file' => 'InternetChecksum.h'
			}
			]
		},
		{
			'vname' => 'TCP over IPv6',
			'name' => 'ipstack_tcp_ipv6',
			'depends' => '&ipstack_tcp and &ipstack_ipv6',
			'subdir' => '/tcp',
			'comp' => [
			{
				'files' => [
					'IPv6_TCP_Receive.ah',
					'IPv6_TCP_Tx_Checksumming.ah'
				]
			},
			{
				'vname' => 'Listen Sockets',
				'name' => 'ipstack_tcp_listen',
				'depends' => '&ipstack_tcp_listen',
				'files' => [
					'IPv6_TCP_Listen.ah'
				]
			},
			{
				'vname' => 'Reset invalid Packets',
				'name' => 'ipstack_tcp_reset',
				'depends' => '&ipstack_tcp_reset',
				'comp' => [
				{
					'files' => [
						'IPv6_TCP_Reset.ah'
					]
				}
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'ARP',
		'name' => 'ipstack_arp',
		'depends' => '&ipstack_arp',
		'subdir' => '/arp',
		'comp' => [
		{
			'files' => [
				'ARP.h',
				'Demux_ARP_Slice.ah',
				'ARP.ah',
				'ARP_Cache.cpp',
				'ARP_Cache.h'
			]
		},
		{
			'depends' => '&ipstack_arp_timeout',
			'file' => 'ARP_Cache_Global_Timeout.ah'
		},
		{
			'depends' => '&ipstack_arp_ipv4 and &ipstack_linklayer_ethernet',
			'file' => 'ARP_Ethernet.ah'
		},
		{
			'depends' => '&ipstack_arp_ipv4_cacheincoming',
			'file' => 'IPv4_ARP_CacheIncoming.ah'
		},
		{
			'vname' => 'ARP - IPv4 over Ethernet',
			'name' => 'ipstack_arp_ipv4',
			'depends' => '&ipstack_arp_ipv4 and &ipstack_linklayer_ethernet and &ipstack_ipv4',
			'subdir' => '/ipv4',
			'comp' => [
			{
				'files' => [
					'ARP_Cache_IPv4_Slice.ah',
					'IPv4_ARP.ah'
				]
			},
			{
				'vname' => 'ARP Request or Reply',
				'name' => 'ipstack_arp_ipv4_requestorreply',
				'depends' => '&ipstack_arp_ipv4_request or &ipstack_arp_ipv4_reply',
				'files' => [
					'Eth_ARP_IPv4_Packet.h',
					'IPv4_ARP_Send_Receive.ah',
					'ARP_Cache_IPv4_Send_Receive_Slice.ah'
				]
			},
			{
				'vname' => 'ARP Request',
				'name' => 'ipstack_arp_ipv4_request',
				'depends' => '&ipstack_arp_ipv4_request',
				'files' => [
					'ARP_Cache_IPv4_Send_Slice.ah',
					'IPv4_Ethernet_ARP.ah',
					'IPv4_ARP_Send.ah'
				]
			},
			{
				'vname' => 'ARP Reply',
				'name' => 'ipstack_arp_ipv4_reply',
				'depends' => '&ipstack_arp_ipv4_reply',
				'file' => 'IPv4_ARP_Receive.ah'
			},
			{
				'vname' => 'Static Cache Entries',
				'name' => 'ipstack_arp_ipv4_static',
				'depends' => '&ipstack_arp_ipv4_static',
					'files' => [
					'ARP_Cache_IPv4_Static_Slice.ah',
					'IPv4_ARP_Static.ah'
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'ICMP over IPv4',
		'name' => 'ipstack_icmp_ipv4',
		'depends' => '&ipstack_icmp_ipv4 and &ipstack_ipv4',
		'subdir' => '/icmpv4',
		'comp' => [
		{
			'files' => ['Demux_IPv4_ICMP_Slice.ah', 'ICMP.h', 'ICMPv4_Socket.cpp', 'ICMPv4_Socket.h', 'ICMPv4_Socket.ah', 'IPv4_ICMP_Receive.ah', 'Sendbuffer_icmp.ah', 'Sendbuffer_icmp_checksum.ah']
		},
		{
			'vname' => 'ICMP over IPv4 - Echo Reply',
			'name' => 'ipstack_icmp_ipv4_echoreply',
			'depends' => '&ipstack_icmp_ipv4_echoreply',
			'comp' => [
			{
				'files' => [
					'IPv4_ICMP_Echo_Reply.ah'
				]
			}
			]
		},
		{
			'vname' => 'Debug out - ICMPv4',
			'name' => 'ipstack_icmp_ipv4_debugout',
			'depends' => '&ipstack_icmp_ipv4_debugout',
			'comp' => [
			{
				'files' => [
					'IPv4_ICMP_ReceiveOutput.ah'
				]
			}
			]
		},
		{
			'vname' => 'ICMP over IPv4 - Destination Unreachable',
			'name' => 'ipstack_icmp_ipv4_destinationunreachable',
			'depends' => '&ipstack_icmp_ipv4_destinationunreachable',
			'comp' => [
			{
				'vname' => 'ICMP over IPv4 - Destination Unreachable - Protocol Unreachable',
				'name' => 'ipstack_icmp_ipv4_destinationunreachable_protocolunreachable',
				'depends' => '&ipstack_icmp_ipv4_destinationunreachable_protocolunreachable',
				'comp' => [
				{
					'files' => [
						'IPv4_ICMP_Protocol_Unreachable.ah'
					]
				}
				]
			},
			{
				'vname' => 'ICMP over IPv4 - Destination Unreachable - Port Unreachable',
				'name' => 'ipstack_icmp_ipv4_destinationunreachable_portunreachable',
				'depends' => '&ipstack_icmp_ipv4_destinationunreachable_portunreachable and &ipstack_udp',
				'comp' => [
				{
					'files' => [
						'IPv4_ICMP_Port_Unreachable.ah'
					]
				}
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'ICMP over IPv6',
		'name' => 'ipstack_icmp_ipv6',
		'depends' => '&ipstack_icmp_ipv6 and &ipstack_ipv6',
		'subdir' => '/icmpv6',
		'comp' => [
		{
			'files' => ['Demux_IPv6_ICMP_Slice.ah', 'ICMPv6.h', 'ICMPv6_Socket.cpp', 'ICMPv6_Socket.h', 'ICMPv6_Socket.ah', 'IPv6_ICMP_Receive.ah', 'SendBuffer_icmpv6.ah', 'Sendbuffer_icmpv6_checksum.ah']
		},
		{
			'depends' => '&ipstack_debug_delay_send',
			'subdir' => '/delay_send',
			'files' => ['DelaySend.ah', 'DelaySendMethod.h', 'DelaySendMethod.cpp']
		},
		{
			'depends' => '&ipstack_debug_delay_send_debug',
			'subdir' => '/delay_send',
			'files' => ['DelaySendDebug.ah']
		},
		{
			'depends' => '&ipstack_ipv6_address_autoconfiguration',
			'subdir' => '/ndp_router',
			'files' => ['Router_Advertisement.ah', 'Router_Solicitation.ah', 'NDPRouterMessages.h', 'RefreshNDPCache_routers.ah']
		},
		{
			'depends' => '&ipstack_ndpcache_ipv6_reply',
			'subdir' => '/ndp_addr_resolve',
			'files' => ['AddressResolutionResponse.ah']
		},
		{
			'depends' => '&ipstack_ndpcache_ipv6_request_and_store',
			'subdir' => '/ndp_addr_resolve',
			'files' => ['AddressResolutionRequest.ah', 'AddressResolutionUpdate.ah']
		},
		{
			'depends' => '&ipstack_icmp_ipv6_ndp_redirect',
			'subdir' => '/ndp_redirect',
			'files' => ['NDP_Redirect.ah']
		},
		{
			'depends' => '&ipstack_ipv6_address_duplication',
			'subdir' => '/ndp_addr_duplication',
			'files' => ['Address_Duplication.ah']
		},
		{
			'depends' => '&ipstack_ipv6_address_expire',
			'subdir' => '/ndp_addr_resolve',
			'files' => ['RefreshNDPCache_non_routers.ah']
		},
		{
			'vname' => 'NDP Basics',
			'depends' => '&ipstack_ndpcache_ipv6_request_and_store or &ipstack_ipv6_address_autoconfiguration or &ipstack_ndpcache_ipv6_reply or &ipstack_icmp_ipv6_ndp_redirect or &ipstack_ipv6_address_expire',
			'subdir' => '/ndp',
			'files' => ['NDPMessages.h', 'NDPNeighborMessages.h', 'NDPNeighborMessages.cpp', 'Demux_Neighbor_Advertisement_Slice.ah', 'Demux_Neighbor_Solicitation_Slice.ah', 'Neighbor_Advertisement_Receive.ah', 'Neighbor_Solicitation_Receive.ah']
		},
		{
			'vname' => 'ICMPv6 - MLD',
			'name' => 'ipstack_icmp_ipv6_multicast_listener_discovery',
			'depends' => '&ipstack_icmp_ipv6_multicast_listener_discovery',
			'subdir' => '/mld',
			'comp' => [
			{
				'files' => ['MLDMessages.h', 'MDL_Respond.ah']
			}
			]
		},
		{
			'vname' => 'ICMPv6 - MLD1',
			'name' => 'ipstack_icmp_ipv6_multicast_listener_discovery1',
			'depends' => '&ipstack_icmp_ipv6_multicast_listener_discovery1',
			'subdir' => '/mld',
			'comp' => [
			{
				'files' => ['MDLv1_AutoJoinLeave.ah']
			}
			]
		},
		{
			'vname' => 'ICMPv6 - MLD2',
			'name' => 'ipstack_icmp_ipv6_multicast_listener_discovery2',
			'depends' => '&ipstack_icmp_ipv6_multicast_listener_discovery2',
			'subdir' => '/mld',
			'comp' => [
			{
				'files' => ['MDLv2_AutoJoinLeave.ah']
			}
			]
		},
		{
			'vname' => 'ICMP over IPv6 - Echo Reply',
			'name' => 'ipstack_icmp_ipv6_echoreply',
			'depends' => '&ipstack_icmp_ipv6_echoreply',
			'comp' => [
			{
				'files' => [
					'IPv6_ICMP_Echo_Reply.ah'
				]
			}
			]
		},
		{
			'vname' => 'Debug out - ICMPv6',
			'name' => 'ipstack_icmp_ipv6_debugout',
			'depends' => '&ipstack_icmp_ipv6_debugout',
			'comp' => [
			{
				'files' => [
					'IPv6_ICMP_ReceiveOutput.ah'
				]
			}
			]
		},
		{
			'vname' => 'ICMP over IPv6 - Packet too big',
			'name' => 'ipstack_icmp_ipv6_packet_too_big',
			'depends' => '&ipstack_icmp_ipv6_packet_too_big',
			'comp' => [
			{
				'files' => [
					'IPv6_ICMP_Too_Big.ah'
				]
			}
			]
		},
		{
			'depends' => '&ipstack_icmp_ipv6_option_unkown',
			'comp' => [
			{
				'files' => [
					'IPv6_ICMP_Option_Unknown.ah'
				]
			}
			]
		},
		{
			'vname' => 'ICMP over IPv6 - Destination Unreachable',
			'name' => 'ipstack_icmp_ipv6_destinationunreachable',
			'depends' => '&ipstack_icmp_ipv6_destinationunreachable',
			'comp' => [
			{
				'vname' => 'ICMP over IPv6 - Destination Unreachable - Protocol Unreachable',
				'name' => 'ipstack_icmp_ipv6_destinationunreachable_protocolunreachable',
				'depends' => '&ipstack_icmp_ipv6_destinationunreachable_protocolunreachable',
				'comp' => [
				{
					'files' => [
						'IPv6_ICMP_Protocol_Unreachable.ah'
					]
				}
				]
			},
			{
				'vname' => 'ICMP over IPv6 - Destination Unreachable - Port Unreachable',
				'name' => 'ipstack_icmp_ipv6_destinationunreachable_portunreachable',
				'depends' => '&ipstack_icmp_ipv6_destinationunreachable_portunreachable and &ipstack_udp_ipv6',
				'comp' => [
				{
					'files' => [
						'IPv6_ICMP_Port_Unreachable.ah'
					]
				}
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'UDP',
		'name' => 'ipstack_udp',
		'depends' => '&ipstack_udp',
		'subdir' => '/udp',
		'comp' => [
		{
			'files' => [
				'UDP.h',
				'UDP_Socket.h',
				'UDP_Socket.cpp'
			]
		},
		{
			'vname' => 'UDP DebugPort implementation',
			'depends' => '&ipstack_udp_debugport',
			'comp' => [
			{
				'file' => 'UDPDebugPort.h',
			},
			{
				'dir' => 'src/hw/hal',
				'depends' => '&hal_DebugPort_ipstack_udp',
				'generate' => 'alias({classname=>"::ipstack::UDPDebugPort",path=>"src/ipstack/udp/UDPDebugPort.h",incpath=>"ipstack/udp/UDPDebugPort.h", newname=>"DebugPortImpl",nsl=>["hw","hal"],guard=>"__pv_guard_iJ2NiUXRV__hUIrsb_i5_22UTbM9qLhes20"});',
				'file' => 'DebugPortImpl.h'
			},
			],
		},
		{
			'vname' => 'UDP receive',
			'name' => 'ipstack_udp_recv',
			'depends' => '&ipstack_udp_recv',
			'comp' => [
			{
				'files' => [
					'Demux_UDP_Slice.ah',
					'UDP_Receive.ah',
					'UDP_Receive_Slice.ah'
				]
			}
			]
		},
		{
			'vname' => 'UDP send',
			'name' => 'ipstack_udp_send',
			'depends' => '&ipstack_udp_send',
			'comp' => [
			{
				'files' => ['SendBuffer_UDP.ah']
			}
			]
		},
		{
			'depends' => '&ipstack_udp_services',
			'files' => ['Management_UDP_Socket.cpp', 'Management_UDP_Socket.h', 'Management_UDP_Socket.ah']
		},
		{
			'depends' => '&ipstack_udp_services_v4',
			'files' => ['../ipv4/udp/UDP_Echo_Reply.ah']
		},
		{
			'depends' => '&ipstack_udp_services_v6',
			'files' => ['../ipv6/udp/UDP_Echo_Reply.ah']
		}
		]
	},
	{
		'vname' => 'TCP',
		'name' => 'ipstack_tcp',
		'depends' => '&ipstack_tcp',
		'subdir' => '/tcp',
		'comp' => [
		{
			'files' => ['Demux_TCP_Slice.ah', 'SendBuffer_TCP.ah', 'TCP.h', 'TCP_Receive.ah', 'TCP_Socket.cpp', 'TCP_Socket.h']
		},
		{
			'depends' => '&ipstack_tcp_listen',
			'files' => ['Demux_TCP_Listen_Slice.ah', 'TCP_Listen.ah']
		},
		{
			'depends' => '&ipstack_tcp_reset',
			'files' => ['Management_TCP_Socket.cpp', 'Management_TCP_Socket.h', 'Management_TCP_Socket.ah']
		},
		{
			'subdir' => '/statemachine',
			'comp' => [
			{
				'files' => [
					'closed.ah',
					'closewait.ah',
					'closing.ah',
					'established.ah',
					'finwait1.ah',
					'finwait2.ah',
					'lastack.ah',
					'listen.ah',
					'synrcvd.ah',
					'synsent.ah',
					'timewait.ah',
					'TCP_Statemachine.ah'
				]
			},
			{
				'depends' => 'not &ipstack_tcp_client',
				'file' => 'synsent_dummy.ah'
			},
			{
				'depends' => 'not &ipstack_tcp_listen',
				'file' => 'listen_dummy.ah'
			}
			]
		},
		{
			'subdir' => '/tcp_history',
			'comp' => [
			{
				'files' => [
					'TCP_History.h',
					'TCP_History.cpp',
					'TCP_Record.h',
					'TCP_Record.cpp'
				]
			},
			{
				'vname' => 'Send Sliding Window',
				'depends' => '&ipstack_tcp_slidingwnd_send',
				'comp' => [
				{
					'files' => [
						'TCP_History_SlidingWindow.ah',
						'TCP_History_SlidingWindow_Slice.ah',
						'TCP_Record_SlidingWindow_Slice.ah'
					]
				}
				]
			},
			{
				'vname' => 'Limit excessive Retransmissions',
				'depends' => '&ipstack_tcp_limit_retransmissions',
				'comp' => [
				{
					'files' => [
						'TCP_Record_RetransmissionCounter.ah',
						'TCP_Record_RetransmissionCounter_Slice.ah'
					]
				}
				]
			}
			]
		},
		{
			'subdir' => '/tcp_receivebuffer',
			'comp' => [
			{
				'files' => [
					'TCP_ReceiveBuffer.cpp',
					'TCP_ReceiveBuffer.h',
					'TCP_RecvElement.h'
				]
			},
			{
				'vname' => 'Receive Sliding Window',
				'depends' => '&ipstack_tcp_slidingwnd_recv',
				'files' => [
					'TCP_ReceiveBuffer_SlidingWindow.ah',
					'TCP_ReceiveBuffer_SlidingWindow_Slice.ah',
					'TCP_RecvElement_SlidingWindow_Slice.ah'
				]
			}
			]
		},
		{
			'vname' => 'TCP Sliding Window Flags Config',
			'comp' => [
			{
				'depends' => '&ipstack_tcp_slidingwnd_send and &ipstack_tcp_slidingwnd_recv',
				'generate' => 'sprintf("#ifndef __TCP_CONFIG_H__\n#define __TCP_CONFIG_H__\n\n#include \"../IPStack_Config.h\"\n\nnamespace ipstack {\nenum {\nTCP_RECEIVEBUFFER_MAX_PACKETS = PACKET_LIMIT, \nTCP_HISTORY_MAX_PACKETS = PACKET_LIMIT };\n}\n\n#endif // __TCP_CONFIG_H__\n")',
				'file' => 'TCP_Config.h'
			},
			{
				'depends' => '&ipstack_tcp_slidingwnd_send and not &ipstack_tcp_slidingwnd_recv',
				'generate' => 'sprintf("#ifndef __TCP_CONFIG_H__\n#define __TCP_CONFIG_H__\n\n#include \"../IPStack_Config.h\"\n\nnamespace ipstack {\nenum {\nTCP_RECEIVEBUFFER_MAX_PACKETS = 1, \nTCP_HISTORY_MAX_PACKETS = PACKET_LIMIT };\n}\n\n#endif // __TCP_CONFIG_H__\n")',

				'file' => 'TCP_Config.h'
			},
			{
				'depends' => 'not &ipstack_tcp_slidingwnd_send and &ipstack_tcp_slidingwnd_recv',
				'generate' => 'sprintf("#ifndef __TCP_CONFIG_H__\n#define __TCP_CONFIG_H__\n\n#include \"../IPStack_Config.h\"\n\nnamespace ipstack {\nenum {\nTCP_RECEIVEBUFFER_MAX_PACKETS = PACKET_LIMIT, \nTCP_HISTORY_MAX_PACKETS = 1 };\n}\n\n#endif // __TCP_CONFIG_H__\n")',
				'file' => 'TCP_Config.h'
			},
			{
				'depends' => 'not &ipstack_tcp_slidingwnd_send and not &ipstack_tcp_slidingwnd_recv',
				'generate' => 'sprintf("#ifndef __TCP_CONFIG_H__\n#define __TCP_CONFIG_H__\n\n#include \"../IPStack_Config.h\"\n\nnamespace ipstack {\nenum {\nTCP_RECEIVEBUFFER_MAX_PACKETS = 1, \nTCP_HISTORY_MAX_PACKETS = 1 };\n}\n\n#endif // __TCP_CONFIG_H__\n")',
				'file' => 'TCP_Config.h'
			}
			]
		},
		{
			'subdir' => '/sws',
			'comp' => [
			{
				'depends' => '&ipstack_tcp_sws_send',
				'files' => [
					'SWS_SenderAvoidance.ah',
					'SWS_SenderAvoidance_Slice.ah'
				]
			},
			{
				'depends' => '&ipstack_tcp_sws_recv',
				'file' => 'SWS_ReceiverAvoidance.ah'
			},
			]
		},
		{
			'vname' => 'TCP Options',
			'subdir' => '/tcp_options',
			'comp' => [
			{
				'depends' => '&ipstack_tcp_mss',
				'files' => [
					'MSS.ah',
					'MSS_TCP_Segment_Slice.ah',
					'MSS_TCP_Socket_Slice.ah'
				]
			}
			]
		},
		{
			'vname' => 'Retransmission Tracker',
			'depends' => '&ipstack_tcp_rtt or &ipstack_tcp_congestionavoidance',
			'subdir' => '/retransmissionTracker',
			'comp' => [
			{
				'vname' => 'TCP Retransmission Tracker',
				'files' => [
					'TCP_RetransmissionTracker.ah',
					'TCP_RetransmissionTrackerSlice.ah'
				]
			}
			]
		},
		{
			'vname' => 'RTT Estimation',
			'subdir' => '/rtt_estimation',
			'depends' => '&ipstack_tcp_rtt',
			'comp' => [
			{
				'vname' => 'Mean and Deviation Meassure',
				'files' => [
					'RTT_Estimation.ah',
					'RTT_Estimation_Slice.ah'
				]
			},
			{
				'vname' => 'Exponential Backoff',
				'depends' => '&ipstack_tcp_rtt_backoff',
				'file' => 'RTT_Exponential_Backoff.ah'
			}
			]
		},
		{
			'vname' => 'Congestion Avoidance',
			'subdir' => '/congestion_avoidance',
			'depends' => '&ipstack_tcp_congestionavoidance',
			'comp' => [
			{
				'vname' => 'Slow Start',
				'depends' => '&ipstack_tcp_congestionavoidance_slowstart',
				'files' => [
					'SlowStart.ah',
					'SlowStart_Slice.ah'
				]
			}
			]
		}
		]
	},
	{
		'vname' => 'Template API',
		'name' => 'ipstack_api',
		'subdir' => '/api',
		'comp' => [
		{
			'depends' => '&ipstack_udp',
			'file' => 'UDP_Socket.h'
		},
		{
			'depends' => '&ipstack_tcp',
			'file' => 'TCP_Socket.h'
		},
		{
			'file' => 'Setup.h'
		}
		]
	},
	]
};
