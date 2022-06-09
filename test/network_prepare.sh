sudo ip netns add tiny-ce-0
sudo ip netns add tiny-ce-1
sudo ip link add tiny-ce-br type bridge
sudo ip link set dev tiny-ce-br up
sudo ip link add ce-0-container type veth peer name ce-0-host
sudo ip link add ce-1-container type veth peer name ce-1-host
sudo ip link set dev ce-0-container netns tiny-ce-0
sudo ip netns exec tiny-ce-0 ip link set dev ce-0-container name eth0
sudo ip netns exec tiny-ce-0 ip addr add 10.0.0.1/24 dev eth0
sudo ip netns exec tiny-ce-0 ip link set dev eth0 up
sudo ip link set dev ce-0-host master tiny-ce-br
sudo ip link set dev ce-0-host up
sudo ip link set dev ce-1-container netns tiny-ce-1
sudo ip netns exec tiny-ce-1 ip link set dev ce-1-container name eth0
sudo ip netns exec tiny-ce-1 ip addr add 10.0.0.2/24 dev eth0
sudo ip netns exec tiny-ce-1 ip link set dev eth0 up
sudo ip link set dev ce-1-host master tiny-ce-br
sudo ip link set dev ce-1-host up
sudo ip link set dev tiny-ce-br down
sudo ip addr add 10.0.1.0/24 dev tiny-ce-br
sudo ip link set dev tiny-ce-br up