use library;
insert into categories (
    category_id
  , category_name
  )
  values
    (1,'SF')
  , (2,'Non-fiction')
  , (3,'Thriller')
  ;
insert into books (
    title
  , author
  , ISBN
  , category_id
  )
  values
    ('The Complete Robot','Asimov, Isaac','978-0-586-05724-7',1)
  , ('Deliver Us From Evil','Baldacci, David','978-0-230-74679-4',3)
  , ('Foundation','Asimov, Isaac','978-0-586-01080-8',1)
  , ('The Escape','Baldacci, David','978-1-4472-6016-5',3)
  ;
