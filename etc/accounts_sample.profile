{
    "users": [
        {
            "userID": "SYSTEMID",
            "encrypted": false,
            "userPassword": ""
        },
        {
            "userID": "",
            "encrypted": false,
            "userPassword": ""
        }
    ],
    "groups": [
        {
            "name": "USER",
            "members": [
                "NTPI"
            ]
        },
        {
            "name": "IMTSERVER",
            "members": [
                "IMTSERVER"
            ]
        }
    ],
    "authorization": [
        {
            "channel": ".*",
            "readGroups": [
                "USER"
            ],
            "writeGroups": [
                "USER"
            ]
        },
        {
            "channel": "COMMON.*",
            "readGroups": [
                "IMTSERVER"
            ],
            "writeGroups": [
                "IMTSERVER"
            ]
        }
    ]
}
