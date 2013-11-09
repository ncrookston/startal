from django.http import HttpResponse
from string import find, split
from subprocess import Popen, PIPE

import re

def get_registered_types(query_type):
  p = Popen(['Sarry/PrintTypes', '--library-path',
    'Sarry/dlls', '--type', query_type], stdout = PIPE)
  (sod, sed) = p.communicate()
  return split(sod)

def get_sarry_command(antenna_type, elevation_type, algorithm_type, flags):
  return ['Sarry/Sarry', '--library-path', 'Sarry/dlls',
    '--antenna-type', antenna_type,
    '--elevation-type', elevation_type,
    '--algorithm-type', algorithm_type] + flags

def get_sarry_output(antenna_type, elevation_type, algorithm_type, flags):
  p = Popen(
    get_sarry_command(antenna_type, elevation_type, algorithm_type, flags),
    stdout = PIPE, stderr = PIPE)
  return p.communicate()

def to_lat_lon_str(lat, lon):
  latStr = str(abs(lat))
  if lat > 0:
    latStr += ' N'
  else:
    latStr += ' S'

  lonStr = str(abs(lon))
  if lon > 0:
    lonStr += ' E'
  else:
    lonStr += ' W'
  return latStr + ', ' + lonStr

def print_kmz(name):
  response = HttpResponse(mimetype='application/vnd.google-earth.kmz')
  response['Content-Disposition'] = 'attachment; filename=src_image.kmz'

  f = open('Sarry/OutputFiles/' + name, 'rb')
  response.write(f.read())
  f.flush()
  return response

def trim_options(options, blacklist = []):
  split_options = re.split(r'[\n\r]+', options)
  blacklist += [('--help', 1), ('--config-file', 3),
    ('Allowed options', 1), ('Required options', 1)]
  new_options = ''
  lines_to_skip = 0
  for line in split_options:
    if lines_to_skip > 0:
      lines_to_skip -= 1
      continue

    doContinue = False
    for blackitem in blacklist:
      if find(line, blackitem[0]) != -1:
        lines_to_skip = blackitem[1] - 1
        doContinue = True
        continue
    if doContinue:
     continue

    new_options += line + '\n'

  return new_options

