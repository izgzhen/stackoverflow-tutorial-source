attack = open("attack_string.txt", 'r').read().split()
cover = open("cover_string.txt", 'r').read().split()

num_NOP = 0
total_len = 2015

num_FF = total_len - num_NOP - len(attack) - len(cover)

result = ['90'] * num_NOP + attack + ['FF'] * num_FF + cover

assert(len(result) == total_len)

print ' '.join(result)
