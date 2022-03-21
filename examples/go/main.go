package main

import (
	"database/sql"
	"fmt"
	"log"

	"github.com/mattn/go-sqlite3"
)

func main() {
	sql.Register("sqlite3_simple",
		&sqlite3.SQLiteDriver{
			Extensions: []string{
				"libsimple-osx-x64/libsimple",
			},
		})

	db, err := sql.Open("sqlite3_simple", ":memory:")
	if err != nil {
		log.Fatal("open error: ", err)
	}
	defer db.Close()

	// db.Exec("create virtual table repo using github(id, full_name, description, html_url)")

	rows, err := db.Query(`select simple_query('pinyin')`)
	if err != nil {
		log.Fatal("query error: ", err)
	}
	defer rows.Close()
	for rows.Next() {
		var query string
		rows.Scan(&query)
		fmt.Printf("%s\n", query)
	}
}
