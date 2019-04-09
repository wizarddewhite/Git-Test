perf stat -e 'kvm:kvm_exit','kvm:kvm_extirq' -a sleep 1h

#Performance counter stats for 'system wide':
#
#        3,358,451      kvm:kvm_exit                                                
#           34,863      kvm:kvm_extirq                                              
#
#    107.488359198 seconds time elapsed

perf record -e 'kvm:kvm_extirq2' -a sleep 1h

# 78.01%  Ext irq2 0 - vec 253 causing vmexit
# 18.05%  Ext irq2 0 - vec 236 causing vmexit
#  2.22%  Ext irq2 124 - vec 33 causing vmexit
#  0.83%  Ext irq2 0 - vec 252 causing vmexit
#  0.69%  Ext irq2 0 - vec 251 causing vmexit
#  0.15%  Ext irq2 125 - vec 33 causing vmexit
#  0.05%  Ext irq2 123 - vec 33 causing vmexit

