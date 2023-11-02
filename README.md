Estimate
=====

An Estimation board for teams. non-persistent memory based. Fast and simple.

![Screenshot-2021-04-16_19-18-36](https://user-images.githubusercontent.com/54403/115060881-ae5f1400-9ee8-11eb-9386-4c5a782fd958.png)

## How it works

- A C server
- CSS with Bulma
- Saves all data in memory
- User can create board with votes options.
- When the server needs the user to have identity it'll redirect him to set his username
- When the board votes changes it'll refresh automatically
- Users can edit boards
- Users can vote on cards
- Users that didn't vote yet are not allowed to show the votes results

## Running the server

Run the server script, it'll install the needed dependencies
```
make run
```

## Guidelines

- No login needed
- No persistent data
- No Javascript (one line to refresh board when updated)
- Minimal dependencies

## Templates syntax

- templates are html files in `views` directory
- C code between `<%` and `%>` will be executed
- C code between `<%=` and `%>` will be executed and is expected to return a C string `char *` which will be printed
- every view file will be compiled to a C function with the name `views_<ile_name>_html`
- every view function takes `void *input` argument and returns `char *`
