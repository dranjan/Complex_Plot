env = DefaultEnvironment(CXX='g++', 
                         CCFLAGS='-ansi -Wall -pedantic -O3 -mtune=native',
                         LIBS=['m', 'SDL'])

ex = ARGUMENTS.get('ex', '00')
cpl_env = env.Clone(CPPDEFINES={'EXAMPLE' : 'example_' + ex})

cpl_env.Program('cpl', 'cplot_examples.cpp')
Program('mandel', 'mandel.cpp')
Program('newton01', 'newton01.cpp')
Program('mandel_sine', 'mandel_sine.cpp')
Program('mandel_ca', 'mandel_ca.cpp')
Program('mandel_ext', 'mandel_ext.cpp')
