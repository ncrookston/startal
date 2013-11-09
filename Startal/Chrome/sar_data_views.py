from django.core.files.base import ContentFile
from django.shortcuts import render_to_response
from django.http import HttpResponse
from Startal.Chrome.models import ElevationData, SarData
from Startal.Chrome.utility import get_registered_types, get_sarry_output, trim_options
from uuid import uuid4
from string import split


def sar_data_tab(request):
  elevation_data = ElevationData.objects.all()
  sar_data = SarData.objects.all()
  antenna_list = get_registered_types('antenna')
  return render_to_response('sar_data/sar_data_tab.html',
    {
      'elevation_list': elevation_data,
      'sar_data_list': sar_data,
      'antenna_list': antenna_list,
    })

def get_antenna_params(request, antenna_type = None, elevation_id = None):
  elevation_data = ElevationData.objects.filter(id = elevation_id)
  if len(elevation_data) == 0:
    return HttpResponse('Invalid elevation data')

  (sod, sed) = get_sarry_output(antenna_type, elevation_data[0].model, 'unused',
      split(elevation_data[0].flags) + ['--help-antenna',])

  return HttpResponse(trim_options(sed))

def add_sar_data(request, name = None, antenna_type = None,
    antenna_flags = None, elevation_id = None):
  elevation_data = ElevationData.objects.filter(id=elevation_id)
  if len(elevation_data) == 0:
    return HttpResponse('invalid elevation data')

  elevation_datum = elevation_data[0]
  (sod, sed) = get_sarry_output(antenna_type, elevation_datum.model, 'summary',
      split(elevation_datum.flags) + split(antenna_flags))

  filename = str(uuid4()) + '.kmz'
  content_file = ContentFile(sod)

  data = SarData(name = name, model = antenna_type, flags = antenna_flags)
  data.overview_kmz.save(filename, content_file)
  data.save()
  return HttpResponse(sed)

def remove_sar_data(request, sar_data_id = None):
  sar_data = SarData.objects.filter(id=sar_data_id)
  if len(sar_data) == 0:
    return HttpResponse('sar data not found')

  sar_data[0].delete()

  return HttpResponse('deleted')

