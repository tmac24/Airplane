//
//  ViewController.swift
//  Airplane
//
//  Created by sunt on 2026/3/4.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
        
        let wrapper = CppWrapper()
        let result = wrapper.add(withA: 3, b: 5)
        print(result)
        
        let result1 = wrapper.add(withA1: 8, b: 10)
        print(result1)

        //纯 C 接口桥接
        let engine = engine_create()
        engine_start(engine)
        engine_destroy(engine)
        
    }


}

