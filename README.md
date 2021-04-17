Estimate
=====

An Estimation board for teams. non-persistent memory based. Fast and simple. ~100 ruby lines.

![Screenshot-2021-04-16_19-18-36](https://user-images.githubusercontent.com/54403/115060881-ae5f1400-9ee8-11eb-9386-4c5a782fd958.png)

## How it works

- A ruby server with Sinatra
- CSS with Bulma
- Saves all data in memory
- User can create board with votes options.
- When the server needs the user to have identity it'll redirect him to set his username and assign uuid
- When the board votes changes it'll refresh automatically
- Users can edit boards
- Users can vote on cards
- Users that didn't vote yet are not allowed to show the votes results

## Running the server

Run the server script, it'll install the needed dependencies
```
bundle install
./server
```

Using Docker

```
docker build -t estimate .
docker run -d -p 3000:3000 estimate
```

Using Docker hub prebuilt image

```
docker run -d -p 3000:3000 emadelsaid/estimate
```

## Deploy to Heroku

You can deploy it directly to Heroku, it must be one process as it's in memory storage

[![Deploy](https://www.herokucdn.com/deploy/button.svg)](https://heroku.com/deploy)

## Guidelines

- No login needed
- No persistent data
- No Javascript (one line to refresh board when updated)
- Minimal dependencies
