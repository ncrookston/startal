from django.shortcuts import render_to_response
from django.http import HttpResponse
from Startal.Chrome.models import ElevationData
from Startal.Chrome.utility import get_registered_types, get_sarry_output, trim_options

def elevation_tab(request):
  data = ElevationData.objects.all()
  elevation_types = get_registered_types('elevation')
  return render_to_response('elevation/elevation_tab.html',
    {
      'elevation_list': data,
      'elevation_types': elevation_types,
    })

def get_elevation_params(request, elevation_type = None):
  (sod, sed) = get_sarry_output('unused', elevation_type, 'unused',
      ['--help-elevation',])
  return HttpResponse(trim_options(sed))

def add_elevation_data(request, name = None, elevation_type = None,
    elevation_flags = None):
  data = ElevationData(name = name, model = elevation_type,
      flags = elevation_flags)
  data.save()
  return HttpResponse('added')

def remove_elevation(request, elevation_id = None):
  data = ElevationData.objects.filter(id = elevation_id)
  if len(data) == 0:
    return HttpResponse('not found')

  data[0].delete()
  return HttpResponse('deleted')

