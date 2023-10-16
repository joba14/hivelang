
#!/usr/bin/env python3

# Modules and libraries
import os
import argparse
import subprocess
import shlex
import filecmp


class Style():
	BLACK = '\033[30m'
	RED = '\033[31m'
	GREEN = '\033[32m'
	YELLOW = '\033[33m'
	BLUE = '\033[34m'
	MAGENTA = '\033[35m'
	CYAN = '\033[36m'
	WHITE = '\033[37m'
	UNDERLINE = '\033[4m'
	RESET = '\033[0m'


# Parsing command line arguments
parser = argparse.ArgumentParser()

parser.add_argument(
	'--settings',
	action='store',
	type=str,
	required=True,
	help='settings for the tester')

options = vars(parser.parse_args())


# Validating settings flag
settings_file_path = options['settings']

if not os.path.isfile(settings_file_path):
	print('Invalid file passed as `--settings` flag!')
	parser.print_help()
	exit(1)

if not settings_file_path.endswith('.json'):
	print('Settings file must be .json file!')
	parser.print_help()
	exit(1)


# Reading settings
import json

with open(settings_file_path, 'r') as settings_file:
	settings = json.load(settings_file)


# Performing tests
passed = 0
failed = 0

for file in os.listdir('./'):
	file_name = os.fsdecode(file)

	if not file_name.endswith(settings['extentions']['source']):
		continue

	test_name              = file_name[:-6]
	test_path              = f'./{test_name}'

	source_extention       = settings['extentions']['source']
	source_file            = f'{test_path}{source_extention}'

	intermediate_extention = settings['extentions']['intermediate']
	intermediate_file      = f'{test_path}{intermediate_extention}'

	object_extention       = '.o'
	object_file            = f'{test_path}{object_extention}'

	output_extention       = '.out'
	output_file            = f'{test_path}{output_extention}'

	expected_extention     = settings['extentions']['expected']
	expected_file          = f'{test_path}{expected_extention}'

	test_config = settings['tests'][test_name]
	if test_config is None:
		print(f'Encountered a test that is not defined in setting.json file! Test name: {test_name}. Skipping...')
		continue

	if test_config['exclude']:
		print(f'Test {test_name} was set to be excluded in the settings. Skipping...')
		continue

	print(f'Testing {source_file}:')

	subprocess.run([ settings['hivec'], '-o', os.path.abspath(intermediate_file), os.path.abspath(source_file) ])
	subprocess.run([ 'nasm', '-felf64', os.path.abspath(intermediate_file) ])
	subprocess.run([ 'ld', '-o', os.path.abspath(output_file), os.path.abspath(object_file) ])

	temp_extention         = '.hlang.temp'
	temp_file              = f'{test_path}{temp_extention}'

	subprocess.run([ 'touch', os.path.abspath(temp_file) ])
	with open(temp_file, 'w') as f:
		subprocess.run([ os.path.abspath(output_file), *test_config['args'] ], stdout=f, stderr=subprocess.DEVNULL)

	if filecmp.cmp(expected_file, temp_file):
		print(Style.GREEN + ' Passed' + Style.RESET)
		passed += 1
	else:
		print(Style.RED + ' Failed' + Style.RESET)
		failed += 1

	if test_config['cleanup']:
		if os.path.isfile(temp_file):
			subprocess.run([ 'rm', '-f', os.path.abspath(temp_file) ])

		if os.path.isfile(intermediate_file):
			subprocess.run([ 'rm', '-f', os.path.abspath(intermediate_file) ])

		if os.path.isfile(object_file):
			subprocess.run([ 'rm', '-f', os.path.abspath(object_file) ])

		if os.path.isfile(output_file):
			subprocess.run([ 'rm', '-f', os.path.abspath(output_file) ])

print('Results:')
print(' ' + Style.GREEN + f'{passed}' + ' tests passed' + Style.RESET + ', ' + Style.RED + f'{failed}' + ' tests failed' + Style.RESET)
