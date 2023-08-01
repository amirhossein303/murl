# mURL (my cURL)

This program is written in c and it's just to test my *C Language knowlege*

as you might guess this is just a very simple tool like **curl** that wrote by me (*so it's called mURL*) that only supports HTTP protocol


## Installation

there is a Makefile here, so just run `make build` to build the ELF (executable) file
```
$ make build
building...
done!
```

and the executable file has build as `murl`

Check out that it was build correct
```
$ ./murl --verion
0.0.1
```

Ok, to get a help of the arguments and CLI 
```
$ ./murl --help
Usage: murl [options...] <url>

Arguments:
 -H, --header               Headers of the request, e.g: 'Accept: plain/text'
 -m, --method               HTTP method for the request. default: GET
 -d, --data                 Data or body of the request, e.g: 'username=admin'
 -p, --params               Query parameters of the request, e.g: 'param1=value'
 -h, --help                 Show this text
 -v, --version              Version number of the murl
 -r, --show-records         Show all avaiables records. [ a: all | d: ata | h: headers | p: params ], default: a
 -f, --format               Specify output format, [json|csv]. default json
```


## Usage

>NOTE: Right now the main logic did not wrote :/

for a quick review of options, let's take a look
```
$ ./murl https://somesite.com --method POST --data "username=admin" --date "password=1234" -H "Content-type: application/json" --params "action=login" --show-records --format json
[
    {
        "key": "username",
        "value": "admin",
        "type": "data"
    },
    {
        "key": "password",
        "value": "1234",
        "type": "data"
    },
    {
        "key": "Content-type",
        "value": "application/json",
        "type": "header"
    },
    {
        "key": "action",
        "value": "login",
        "type": "parameter"
    },
]
```

note that the `records` are the given {data,headers,params}. check out the source code

It's possible filter records:<br>to get only headers use `--show-records h` which **h** stand for *headers*

## Examples

It's possible to add params to url:
```
./murl 'http://localhost:8080/api/posts?id=1' --params "page=1" --show-records p
[
    {
        "key": "id",
        "value": "1",
        "type": "parameter"
    },
    {
        "key": "page",
        "value": "1",
        "type": "parameter"
    } 
]
```

**Headers**

```
./murl 'http://localhost:8080' -H 'User-Agent: Mozilla/5.0' -H 'Accept: */*' -H 'Accept-Language: en-US,en;q=0.5' -H 'Accept-Encoding: gzip, deflate' -H 'Referer: http://localhost:8080' -H 'Content-Type: application/json' -H 'Origin: http://localhost:8080' -H 'Connection: keep-alive' -H 'Cookie: session=SESSION' -r h
[
    ...
    {
        "key": "Accept-Encoding",
        "value": "gzip, deflate",
        "type": "header"
    },
    {
        "key": "Referer",
        "value": "http",
        "type": "header"
    },
    {
        "key": "Content-Type",
        "value": "application/json",
        "type": "header"
    },
    ...
]
```

## Reports bugs
I know there are some bugs there so it would help me you to report it in issue :)

