import peewee


DB = peewee.SqliteDatabase('data/db.db', pragmas={'foreign_keys': 1})


class BaseModel(peewee.Model):
    class Meta:
        database = DB
