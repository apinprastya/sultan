package main

import (
    "os"
    "io"
    "archive/zip"
    "path/filepath"
    "runtime"
    "os/exec"
)

func main() {
    dir, _ := filepath.Abs(filepath.Dir(os.Args[0]))
    if _, err := os.Stat(dir + "/update/sultan.zip"); !os.IsNotExist(err) {
        Unzip(dir + "/update/sultan.zip", dir)      
    }
    if runtime.GOOS == "windows" {
        c := exec.Command(dir + "/Sultan.exe")
        c.Start()
    } else {
        c := exec.Command(dir + "/Sultan.sh")
        c.Start()
    }
}

func Unzip(src, dest string) error {
    r, err := zip.OpenReader(src)
    if err != nil {
        return err
    }
    defer func() {
        if err := r.Close(); err != nil {
            panic(err)
        }
    }()

    os.MkdirAll(dest, 0755)

    for _, f := range r.File {
        err := extractAndWriteFile(f, dest)
        if err != nil {
            return err
        }
    }

    return nil
}

func extractAndWriteFile(f *zip.File, dest string) error {
    rc, err := f.Open()
    if err != nil {
        return err
    }
    defer func() {
        if err := rc.Close(); err != nil {
            panic(err)
        }
    }()

    path := filepath.Join(dest, f.Name)

    if f.FileInfo().IsDir() {
        os.MkdirAll(path, f.Mode())
    } else {
        os.MkdirAll(filepath.Dir(path), f.Mode())
        f, err := os.OpenFile(path, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, f.Mode())
        if err != nil {
            return err
        }
        defer func() {
            if err := f.Close(); err != nil {
                panic(err)
            }
        }()

        _, err = io.Copy(f, rc)
        if err != nil {
            return err
        }
    }
    return nil
}