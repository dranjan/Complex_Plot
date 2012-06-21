env = DefaultEnvironment(CXX='g++', CC='gcc',
                         CCFLAGS='-ansi -Wall -pedantic -O3 -mtune=native',
                         LIBS=['m', 'SDL'])

ex = ARGUMENTS.get('ex', '00')
cpl_env = env.Clone(CPPDEFINES={'EXAMPLE' : 'example_' + ex})

cpl_env.Program('cpl', 'cplot_examples.cpp')
Program('mandel', 'mandel.cpp')
Program('newton01', 'newton01.cpp')
Program('newton02', 'newton02.cpp')
Program('mandel_sine', 'mandel_sine.cpp')
Program('mandel_ca', 'mandel_ca.cpp')
Program('mandel_ext', ['mandel_ext.cpp', 'itinerary.c'])
Program('mandel_iter', 'mandel_iter.cpp')
Program('mandel_iter02', 'mandel_iter02.cpp')
Program('mandel_ext_anim', 'mandel_ext_anim.cpp')
Program('mandel_iter03', 'mandel_iter03.cpp')
Program('mandel_quad', 'mandel_quad.cpp')
Program('mandel_quad02', 'mandel_quad02.cpp')
Program('mandel_quad03', 'mandel_quad03.cpp')
Program('tan_weird01', 'tan_weird01.cpp')
Program('elliptic_test', 'elliptic_test.cpp')
Program('theta_functions', 'theta_functions.cpp')
