from .base import DB, BaseModel
from .catalog import Catalog
from .size_param import SizeParam
from .sample import Sample


def make_tables():
    DB.create_tables([Catalog, SizeParam])