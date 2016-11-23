drop database library;
create database library;
create table library.categories (
  category_id integer not null auto_increment unique,
  category_name varchar(70),
  primary key (category_id)
  );
create table library.books (
  book_id integer not null auto_increment unique,
  title varchar(70),
  author varchar(70),
  ISBN varchar(17),
  category_id integer,
  primary key (book_id)
  );
create index category_id_key on library.categories (category_id);
create index book_id_key on library.books (book_id);
alter table library.books add index category_id (category_id),
  add constraint category_id foreign key (category_id)
  references library.categories (category_id)
  ;
