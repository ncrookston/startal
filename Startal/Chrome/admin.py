from Startal.Chrome.models import SarData, RegionOfInterest, ProcessingTask, ElevationData

#TODO: It may be convenient to abuse the file-upload capabilities
# of the browser to simplify specifying a path.
#from Startal.Chrome.widgets import PathLoader
from django.contrib import admin

#class sar_collection_admin(admin.ModelAdmin):
#  def formfield_for_dbfield(self, db_field, **kwargs):
#    if db_field.name == 'local_path':
#      kwargs['widget'] = PathLoader
#    return super(sar_collection_admin, self).formfield_for_dbfield(db_field, **kwargs)

admin.site.register(ElevationData)
admin.site.register(SarData)
admin.site.register(RegionOfInterest)
admin.site.register(ProcessingTask)
